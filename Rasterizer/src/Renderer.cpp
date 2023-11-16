//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

namespace dae
{
	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
		m_AspectRatio = static_cast<float>(m_Width) / m_Height;

		//Create Buffers
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

		m_pDepthBufferPixels = std::make_unique<float[]>(m_Width * m_Height);

		m_pTestTexture = Texture::LoadFromFile("Resources/uv_grid_2.png");

		//Initialize Camera
		m_Camera.Initialize(60.f, { 0.0f,0.0f,-10.f });
	}

	Renderer::~Renderer()
	{
		//delete[] m_pDepthBufferPixels;
	}

	void Renderer::Update(Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}

	void Renderer::Render()
	{
		//@START
		std::fill_n(m_pDepthBufferPixels.get(), m_Width * m_Height, FLT_MAX);
		SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

		//Lock BackBuffer
		SDL_LockSurface(m_pBackBuffer);

		std::vector<Mesh> meshes{
			Mesh {
				{
					Vertex{ {-3,  3, -2}, colors::White, {0.0f, 0.0f} },
					Vertex{ { 0,  3, -2}, colors::White, {0.5f, 0.0f} },
					Vertex{ { 3,  3, -2}, colors::White, {1.0f, 0.0f} },
					Vertex{ {-3,  0, -2}, colors::White, {0.0f, 0.5f} },
					Vertex{ { 0,  0, -2}, colors::White, {0.5f, 0.5f} },
					Vertex{ { 3,  0, -2}, colors::White, {1.0f, 0.5f} },
					Vertex{ {-3, -3, -2}, colors::White, {0.0f, 1.0f} },
					Vertex{ { 0, -3, -2}, colors::White, {0.5f, 1.0f} },
					Vertex{ { 3, -3, -2}, colors::White, {1.0f, 1.0f} },
				},
				{
					3, 0, 1,    1, 4, 3,    4, 1, 2,
					2, 5, 4,    6, 3, 4,    4, 7, 6,
					7, 4, 5,    5, 8, 7
				},
				PrimitiveTopology::TriangleList
			},
			//Mesh {
			//	{
			//		Vertex{ {-3,  3, -2} },
			//		Vertex{ { 0,  3, -2} },
			//		Vertex{ { 3,  3, -2} },
			//		Vertex{ {-3,  0, -2} },
			//		Vertex{ { 0,  0, -2} },
			//		Vertex{ { 3,  0, -2} },
			//		Vertex{ {-3, -3, -2} },
			//		Vertex{ { 0, -3, -2} },
			//		Vertex{ { 3, -3, -2} },
			//	},
			//	{
			//		3, 0, 4, 1, 5, 2,
			//		2, 6,
			//		6, 3, 7, 4, 8, 5
			//	},
			//	PrimitiveTopology::TriangleStrip
			//}
		};

		for (auto& mesh : meshes)
		{
			VertexTransformationFunction(mesh.vertices, mesh.vertices_out);

			switch (mesh.primitiveTopology)
			{
				case PrimitiveTopology::TriangleList:
					RasterizeTriangleList(mesh);
					break;

				case PrimitiveTopology::TriangleStrip:
					RasterizeTriangleStrip(mesh);
					break;
			}
		}

		//@END
		//Update SDL Surface
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
	}

	void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const
	{
		vertices_out.resize(vertices_in.size());

		for (size_t i = 0; i < vertices_in.size(); ++i)
		{
			Vertex_Out& vertex = vertices_out[i];

			// Convert Vertex to Vertex_Out
			vertex.position	= { vertices_in[i].position, 0.0f };
			vertex.color	= vertices_in[i].color;
			vertex.uv		= vertices_in[i].uv;

			// Transform to view space
			vertex.position = m_Camera.viewMatrix.TransformPoint(vertex.position);

			// Perspective divide
			vertex.position.x /= vertex.position.z;
			vertex.position.y /= vertex.position.z;

			// Apply camera settings
			vertex.position.x /= m_AspectRatio * m_Camera.fov;
			vertex.position.y /= m_Camera.fov;

			// Transform to screen space
			vertex.position.x = (vertex.position.x + 1) * 0.5f * m_Width;
			vertex.position.y = (1 - vertex.position.y) * 0.5f * m_Height;
		}
	}

	bool Renderer::SaveBufferToImage() const
	{
		return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
	}

	void Renderer::RasterizeTriangleStrip(const Mesh& mesh)
	{
		for (size_t i = 2; i < mesh.indices.size(); ++i)
		{
			size_t i0 = i - ((i % 2) == 0 ? 2 : 1);
			size_t i1 = i - ((i % 2) == 0 ? 1 : 2);
			size_t i2 = i;

			const Vertex_Out& v0 = mesh.vertices_out[mesh.indices[i0]];
			const Vertex_Out& v1 = mesh.vertices_out[mesh.indices[i1]];
			const Vertex_Out& v2 = mesh.vertices_out[mesh.indices[i2]];

			RasterizeTriangle(v0, v1, v2);
		}
	}

	void Renderer::RasterizeTriangleList(const Mesh& mesh)
	{
		assert(mesh.indices.size() % 3 == 0 && "incomplete triangles");

		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vertex_Out& v0 = mesh.vertices_out[mesh.indices[i]];
			const Vertex_Out& v1 = mesh.vertices_out[mesh.indices[i + 1]];
			const Vertex_Out& v2 = mesh.vertices_out[mesh.indices[i + 2]];

			RasterizeTriangle(v0, v1, v2);
		}
	}

	void Renderer::RasterizeTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2)
	{
		// Find triangle bounding box
		auto boxLeft	= static_cast<int>(std::min({ v0.position.x, v1.position.x, v2.position.x })) - 1;
		auto boxTop		= static_cast<int>(std::min({ v0.position.y, v1.position.y, v2.position.y })) - 1;
		auto boxRight	= static_cast<int>(std::max({ v0.position.x, v1.position.x, v2.position.x })) + 1;
		auto boxBottom	= static_cast<int>(std::max({ v0.position.y, v1.position.y, v2.position.y })) + 1;

		boxLeft			= std::max(0, boxLeft);
		boxTop			= std::max(0, boxTop);
		boxRight		= std::min(m_Width, boxRight);
		boxBottom		= std::min(m_Height, boxBottom);

		// Calculate triangle edges
		Vector2 e0 = (v1.position - v0.position).GetXY();
		Vector2 e1 = (v2.position - v1.position).GetXY();
		Vector2 e2 = (v0.position - v2.position).GetXY();

		for (int px = boxLeft; px < boxRight; ++px)
		{
			for (int py = boxTop; py < boxBottom; ++py)
			{
				int bufferIndex = px + py * m_Width;
				Vector2 pixel{ px + 0.5f, py + 0.5f };

				assert(bufferIndex < m_Width * m_Height && "buffer index out of bounds");

				// Calculate vertex to pixel vectors
				Vector2 p0 = pixel - v0.position.GetXY();
				Vector2 p1 = pixel - v1.position.GetXY();
				Vector2 p2 = pixel - v2.position.GetXY();

				// Barycentric cooridnates (weights)
				float invTotalWeight = 1.0f / Vector2::Cross(e0, -e2);
				float w0 = Vector2::Cross(e1, p1) * invTotalWeight;
				float w1 = Vector2::Cross(e2, p2) * invTotalWeight;
				float w2 = Vector2::Cross(e0, p0) * invTotalWeight;

				bool sign0 = std::signbit(w0);
				bool sign1 = std::signbit(w1);
				bool sign2 = std::signbit(w2);

				// Check sign equality,
				// - false: pixel inside triangle
				// - true: pixel outside triangle
				if (sign0 != sign1 || sign1 != sign2) continue;

				// Interpolate depth value using weights
				float depth = 1.0f / (w0 / v0.position.z + w1 / v1.position.z + w2 / v2.position.z);

				// Depth test
				// - false: pixel in front
				// - true: pixel behind object
				if (depth > m_pDepthBufferPixels[bufferIndex]) continue;

				// Interpolate vertex colors using weights
				ColorRGB color = v0.color * w0 + v1.color * w1 + v2.color * w2;
				color.MaxToOne();

				Vector2 uv = (v0.uv / v0.position.z * w0 + v1.uv / v1.position.z * w1 + v2.uv / v2.position.z * w2) * depth;
				if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) continue;
				color *= m_pTestTexture->Sample(uv);

				m_pBackBufferPixels[bufferIndex] = SDL_MapRGB(
					m_pBackBuffer->format,
					static_cast<uint8_t>(color.r * 255),
					static_cast<uint8_t>(color.g * 255),
					static_cast<uint8_t>(color.b * 255)
				);

				m_pDepthBufferPixels[bufferIndex] = depth;
			}
		}
	}
}
