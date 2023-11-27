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

		//Initialize Camera
		m_Camera.Initialize(m_AspectRatio, 60.f, { 0.0f, 0.0f, -10.f });
		
		Mesh tuktuk;
		Utils::ParseOBJ("Resources/tuktuk.obj", tuktuk.vertices, tuktuk.indices);
		tuktuk.texture = Texture::LoadFromFile("Resources/tuktuk.png");
		tuktuk.primitiveTopology = PrimitiveTopology::TriangleList;

		m_Meshes.emplace_back(std::move(tuktuk));
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

		for (auto& mesh : m_Meshes)
		{
			VertexTransformationFunction(mesh);

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

	void Renderer::VertexTransformationFunction(Mesh& mesh) const
	{
		const auto& verticesIn = mesh.vertices;
		auto& verticesOut = mesh.vertices_out;

		Matrix wvp = mesh.worldMatrix * m_Camera.viewMatrix * m_Camera.projectionMatrix;

		verticesOut.resize(verticesIn.size());

		for (size_t i = 0; i < verticesIn.size(); ++i)
		{
			Vertex_Out& vertex = verticesOut[i];

			// Convert Vertex to Vertex_Out
			vertex.position	= { verticesIn[i].position, 1.0f };
			vertex.color	= verticesIn[i].color;
			vertex.uv		= verticesIn[i].uv;
			vertex.normal	= verticesIn[i].normal;
			vertex.tangent	= verticesIn[i].tangent;

			vertex.position = wvp.TransformPoint(vertex.position);

			// Perspective divide
			vertex.position.x /= vertex.position.w;
			vertex.position.y /= vertex.position.w;
			vertex.position.z /= vertex.position.w;

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

			RasterizeTriangle(v0, v1, v2, mesh.texture.get());
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

			RasterizeTriangle(v0, v1, v2, mesh.texture.get());
		}
	}

	void Renderer::RasterizeTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2, Texture* pTexture)
	{
		// Find triangle bounding box
		auto boxLeft	= static_cast<int>(std::min({ v0.position.x, v1.position.x, v2.position.x })) - 1;
		auto boxTop		= static_cast<int>(std::min({ v0.position.y, v1.position.y, v2.position.y })) - 1;
		auto boxRight	= static_cast<int>(std::max({ v0.position.x, v1.position.x, v2.position.x })) + 1;
		auto boxBottom	= static_cast<int>(std::max({ v0.position.y, v1.position.y, v2.position.y })) + 1;

		if (boxLeft < 0.0f || boxRight > m_Width || boxTop < 0.0f || boxBottom > m_Height) return;

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

				// Check sign equality
				// - false: pixel inside triangle
				// - true: pixel outside triangle
				if (sign0 != sign1 || sign1 != sign2) continue;

				// Interpolate depth value using weights
				float depthZ = 1.0f / (w0 / v0.position.z + w1 / v1.position.z + w2 / v2.position.z);
				float depthW = 1.0f / (w0 / v0.position.w + w1 / v1.position.w + w2 / v2.position.w);

				// Frustum culling
				// - false: inside frustum
				// - true: outside frustum
				if (depthZ < 0.0f || depthZ > 1.0f) continue;

				// Depth test
				// - false: pixel in front
				// - true: pixel behind object
				if (depthZ > m_pDepthBufferPixels[bufferIndex]) continue;

				// Interpolate vertex colors using weights
				ColorRGB color = v0.color * w0 + v1.color * w1 + v2.color * w2;
				color.MaxToOne();
				
				if (pTexture != nullptr)
				{
					Vector2 uv = (v0.uv / v0.position.w * w0 + v1.uv / v1.position.w * w1 + v2.uv / v2.position.w * w2) * depthW;
					if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) continue;
					color *= pTexture->Sample(uv);
				}

				m_pBackBufferPixels[bufferIndex] = SDL_MapRGB(
					m_pBackBuffer->format,
					static_cast<uint8_t>(color.r * 255),
					static_cast<uint8_t>(color.g * 255),
					static_cast<uint8_t>(color.b * 255)
				);

				m_pDepthBufferPixels[bufferIndex] = depthZ;
			}
		}
	}
}
