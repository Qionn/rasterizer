#pragma once

#include <memory>
#include <string>

#include "Shader.h"
#include "Texture.h"
#include "Maths.h"

namespace dae
{
	class LitShader final : public Shader
	{
	public:
		LitShader() = default;
		~LitShader() = default;

		LitShader(const LitShader&)				= delete;
		LitShader& operator=(const LitShader&)	= delete;
		LitShader(LitShader&&)					= delete;
		LitShader& operator=(LitShader&&)			= delete;

		bool CanShade(Vertex_Out& vertex) const override;
		ColorRGB Shade(Vertex_Out& vertex) const override;

		void SetDiffuseTexture(const std::string& texturePath);
		void SetNormalTexture(const std::string& texturePath);
		void SetGlossTexture(const std::string& texturePath);
		void SetSpecularTexture(const std::string& texturePath);

		void SetAlphaClipping(float clipping);

	private:
		std::unique_ptr<Texture> m_pDiffuseTexture;
		std::unique_ptr<Texture> m_pNormalTexture;
		std::unique_ptr<Texture> m_pGlossTexture;
		std::unique_ptr<Texture> m_pSpecularTexture;

		Vector3 m_GlobalLightDirection{ 0.577f, -0.577f, 0.577f };
		float m_Shininess{ 25.0f };
		float m_AlphaClipping{ 0.0f };
	};
}
