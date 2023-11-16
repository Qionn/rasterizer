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
					Vertex{ {-3,  3, -2} },
					Vertex{ { 0,  3, -2} },
					Vertex{ { 3,  3, -2} },
					Vertex{ {-3,  0, -2} },
					Vertex{ { 0,  0, -2} },
					Vertex{ { 3,  0, -2} },
					Vertex{ {-3, -3, -2} },
					Vertex{ { 0, -3, -2} },
					Vertex{ { 3, -3, -2} },
				},
				{
					3, 0, 1,    1, 4, 3,    4, 1, 2,
					2, 5, 4,    6, 3, 4,    4, 7, 6,
					7, 4, 5,    5, 8, 7
				},
				PrimitiveTopology::TriangleList
			}
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
			vertex.position = { vertices_in[i].position, 0.0f };
			vertex.color = vertices_in[i].color;

			TransformToViewSpace(vertex.position);
			PerspectiveDivide(vertex.position);
			ApplyCameraSettings(vertex.position);
			TransformToScreenSpace(vertex.position);
		}
	}

	bool Renderer::SaveBufferToImage() const
	{
		return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
	}

	void Renderer::RasterizeTriangleStrip(const Mesh& mesh)
	{

	}

	void Renderer::RasterizeTriangleList(const Mesh& mesh)
	{
		assert(mesh.indices.size() % 3 == 0 && "incomplete triangle");

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
		auto boxLeft = static_cast<int>(std::min({ v0.position.x, v1.position.x, v2.position.x }));
		auto boxTop = static_cast<int>(std::min({ v0.position.y, v1.position.y, v2.position.y }));
		auto boxRight = static_cast<int>(std::max({ v0.position.x, v1.position.x, v2.position.x }));
		auto boxBottom = static_cast<int>(std::max({ v0.position.y, v1.position.y, v2.position.y }));

		boxLeft = std::max(0, boxLeft);
		boxTop = std::max(0, boxTop);
		boxRight = std::min(m_Width, boxRight);
		boxBottom = std::min(m_Height, boxBottom);

		for (int px = boxLeft; px < boxRight; ++px)
		{
			for (int py = boxTop; py < boxBottom; ++py)
			{
				Vector2 pixel{ px + 0.5f, py + 0.5f };
				float w0, w1, w2;

				if (!IsPixelInsideTriangle(pixel, v0.position, v1.position, v2.position, w0, w1, w2)) continue;

				float depth = v0.position.z * w0 + v1.position.z * w1 + v2.position.z * w2;
				if (!PerformDepthTest(px, py, depth)) continue;

				ColorRGB color = v0.color * w0 + v1.color * w1 + v2.color * w2;
				color.MaxToOne();

				WritePixel(px, py, color, depth);
			}
		}
	}

	bool Renderer::IsPixelInsideTriangle(const Vector2& pixel, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& w0, float& w1, float& w2) const
	{
		Vector2 V0V1 = (v1 - v0).GetXY();
		Vector2 V1V2 = (v2 - v1).GetXY();
		Vector2 V2V0 = (v0 - v2).GetXY();

		Vector2 V0P = pixel - v0.GetXY();
		Vector2 V1P = pixel - v1.GetXY();
		Vector2 V2P = pixel - v2.GetXY();

		float invTotalWeight = 1.0f / Vector2::Cross(V0V1, -V2V0);
		w0 = Vector2::Cross(V1V2, V1P) * invTotalWeight;
		w1 = Vector2::Cross(V2V0, V2P) * invTotalWeight;
		w2 = Vector2::Cross(V0V1, V0P) * invTotalWeight;

		bool sign1 = std::signbit(w0);
		bool sign2 = std::signbit(w1);
		bool sign3 = std::signbit(w2);

		return (sign1 == sign2 && sign2 == sign3);
	}

	bool Renderer::PerformDepthTest(int px, int py, float depth) const
	{
		int index = px + py * m_Width;
		assert(index < m_Width * m_Height && "index out of bounds");
		return depth < m_pDepthBufferPixels[index];
	}

	void Renderer::WritePixel(int px, int py, const ColorRGB& color, float depth)
	{
		int index = px + py * m_Width;
		assert(index < m_Width * m_Height && "index out of bounds");

		m_pBackBufferPixels[index] = SDL_MapRGB(
			m_pBackBuffer->format,
			static_cast<uint8_t>(color.r * 255),
			static_cast<uint8_t>(color.g * 255),
			static_cast<uint8_t>(color.b * 255)
		);

		m_pDepthBufferPixels[index] = depth;
	}

	void Renderer::TransformToViewSpace(Vector4& vertex) const
	{
		vertex = m_Camera.viewMatrix.TransformPoint(vertex);
	}

	void Renderer::PerspectiveDivide(Vector4& vertex) const
	{
		vertex.x /= vertex.z;
		vertex.y /= vertex.z;
	}

	void Renderer::ApplyCameraSettings(Vector4& vertex) const
	{
		vertex.x /= m_AspectRatio * m_Camera.fov;
		vertex.y /= m_Camera.fov;
	}

	void Renderer::TransformToScreenSpace(Vector4& vertex) const
	{
		vertex.x = (vertex.x + 1) * 0.5f * m_Width;
		vertex.y = (1 - vertex.y) * 0.5f * m_Height;
	}
}
