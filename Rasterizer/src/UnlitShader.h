#pragma once

#include <memory>
#include <string>

#include "Shader.h"
#include "Texture.h"

namespace dae
{
	class UnlitShader final : public Shader
	{
	public:
		UnlitShader() = default;
		~UnlitShader() = default;

		UnlitShader(const UnlitShader&)				= delete;
		UnlitShader& operator=(const UnlitShader&)	= delete;
		UnlitShader(UnlitShader&&)					= delete;
		UnlitShader& operator=(UnlitShader&&)		= delete;

		bool CanShade(Vertex_Out& vertex) const override;
		ColorRGB Shade(Vertex_Out& vertex) const override;

		void SetDiffuseTexture(const std::string& texturePath);
		void SetAlphaClipping(float clipping);

	private:
		std::unique_ptr<Texture> m_pDiffuseTexture;
		float m_AlphaClipping{ 0.0f };
	};
}
