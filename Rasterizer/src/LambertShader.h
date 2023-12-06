#pragma once

#include <memory>
#include <string>

#include "Shader.h"
#include "Texture.h"
#include "Maths.h"

namespace dae
{
	class LambertShader final : public Shader
	{
	public:
		LambertShader() = default;
		~LambertShader() = default;

		LambertShader(const LambertShader&)				= delete;
		LambertShader& operator=(const LambertShader&)	= delete;
		LambertShader(LambertShader&&)					= delete;
		LambertShader& operator=(LambertShader&&)			= delete;

		bool CanShade(Vertex_Out& vertex) const override;
		ColorRGB Shade(Vertex_Out& vertex) const override;

		void SetDiffuseTexture(const std::string& texturePath);
		void SetNormalTexture(const std::string& texturePath);
		void SetGlossTexture(const std::string& texturePath);
		void SetSpecularTexture(const std::string& texturePath);

		void SetAmbientLight(const ColorRGB& color);
		void SetLightDirection(const Vector3& direction);
		void SetDiffuseReflection(float kd);
		void SetShininess(float shininess);
		void SetAlphaClipping(float clipping);

	private:
		std::unique_ptr<Texture> m_pDiffuseTexture;
		std::unique_ptr<Texture> m_pNormalTexture;
		std::unique_ptr<Texture> m_pGlossTexture;
		std::unique_ptr<Texture> m_pSpecularTexture;

		ColorRGB m_AmbientLight{ 0.0f, 0.0f, 0.0f};
		Vector3 m_LightDirection{ 0.577f, -0.577f, 0.577f };
		float m_DiffuseReflection{ 7.0f };
		float m_Shininess{ 25.0f };
		float m_AlphaClipping{ 0.0f };
	};
}
