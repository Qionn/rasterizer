#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	struct Vertex_Out;
	class Timer;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

		bool SaveBufferToImage() const;
		void ToggleDebugDepthBuffer();
		void ToggleDebugRotation();

		void VertexTransformationFunction(Mesh& mesh) const;

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		std::unique_ptr<float[]> m_pDepthBufferPixels{};

		Camera m_Camera{};
		Vector3 m_GlobalLightDirection{ .577f, -.577f, .577f };

		// ==== DEBUG MESH ====
		Mesh m_TestMesh{};
		std::unique_ptr<Texture> m_pTestAlbedoTexture;
		bool m_RotateTestMesh{ true };
		// ====================

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};
		bool m_DebugDepthBuffer{};

	private:
		void RasterizeTriangleStrip(const Mesh& mesh);
		void RasterizeTriangleList(const Mesh& mesh);
		void RasterizeTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2);

		void ShadePixel(int pixelIndex, const Vertex_Out& v) const;
	};
}
