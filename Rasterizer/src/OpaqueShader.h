#pragma once

#include <memory>
#include <string>

#include "Shader.h"
#include "Texture.h"
#include "Maths.h"

namespace dae
{
	class OpaqueShader final : public Shader
	{
	public:
		OpaqueShader() = default;
		~OpaqueShader() = default;

		OpaqueShader(const OpaqueShader&)				= delete;
		OpaqueShader& operator=(const OpaqueShader&)	= delete;
		OpaqueShader(OpaqueShader&&)					= delete;
		OpaqueShader& operator=(OpaqueShader&&)			= delete;

		ColorRGB Shade(Vertex_Out& vertex) const override;

		void SetDiffuseTexture(const std::string& texturePath);
		void SetNormalTexture(const std::string& texturePath);
		void SetGlossTexture(const std::string& texturePath);
		void SetSpecularTexture(const std::string& texturePath);

	private:
		std::unique_ptr<Texture> m_pDiffuseTexture;
		std::unique_ptr<Texture> m_pNormalTexture;
		std::unique_ptr<Texture> m_pGlossTexture;
		std::unique_ptr<Texture> m_pSpecularTexture;

		Vector3 m_GlobalLightDirection{ 0.577f, -0.577f, 0.577f };
		float m_Shininess{ 25.0f };
	};
}
