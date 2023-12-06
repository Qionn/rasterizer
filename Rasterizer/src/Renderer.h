#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Camera.h"
#include "DataTypes.h"
#include "ColorRGB.h"
#include "Maths.h"

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
	class Shader;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene);

		bool SaveBufferToImage() const;
		float GetAspectRatio() const;

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		std::unique_ptr<float[]> m_pDepthBuffer{};

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};

		Shader* m_pCurrentShader{ nullptr };

	private:
		void VertexTransformationFunction(const Camera& camera, Mesh& mesh) const;

		void RasterizeTriangleStrip(const Mesh& mesh);
		void RasterizeTriangleList(const Mesh& mesh);
		void RasterizeTriangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2);
	};
}
