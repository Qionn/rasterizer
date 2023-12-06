#include "LambertShader.h"

#include "DataTypes.h"

namespace dae
{
	bool LambertShader::CanShade(Vertex_Out& vertex) const
	{
		if (m_AlphaClipping > 0.0f && m_pDiffuseTexture != nullptr)
		{
			return m_pDiffuseTexture->SampleAlpha(vertex.uv) > m_AlphaClipping;
		}

		return true;
	}

	ColorRGB LambertShader::Shade(Vertex_Out& vertex) const
	{
		// Normal calculation
		Vector3 normal = vertex.normal;

		if (m_pNormalTexture != nullptr)
		{
			Vector3 binoral = Vector3::Cross(vertex.normal, vertex.tangent);
			Matrix tangentSpaceMatrix = Matrix{ vertex.tangent, binoral, vertex.normal, Vector3::Zero };
			normal = tangentSpaceMatrix.TransformVector(m_pNormalTexture->SampleNormal(vertex.uv));
		}

		// Diffuse color (lambert)
		ColorRGB color = vertex.color;

		if (m_pDiffuseTexture != nullptr)
		{
			color = m_pDiffuseTexture->SampleColor(vertex.uv);
		}

		float lambertian = std::max(Vector3::Dot(-m_LightDirection, normal), 0.0f);
		ColorRGB lambertianRGB{ lambertian, lambertian, lambertian };

		color *= m_DiffuseReflection / PI * (lambertianRGB + m_AmbientLight);

		// Specular reflection (phong)
		if (m_pGlossTexture != nullptr && m_pSpecularTexture != nullptr)
		{
			float cosa = Vector3::Dot(Vector3::Reflect(-m_LightDirection, normal), vertex.viewDirection);
			if (cosa > 0.0f)
			{
				float phong = m_pGlossTexture->SampleGray(vertex.uv) * std::pow(cosa, m_pSpecularTexture->SampleGray(vertex.uv) * m_Shininess);
				color += ColorRGB{ phong, phong, phong };
			}
		}

		color.MaxToOne();

		return color;
	}

	void LambertShader::SetDiffuseTexture(const std::string& texturePath)
	{
		m_pDiffuseTexture = Texture::LoadFromFile(texturePath);
	}

	void LambertShader::SetNormalTexture(const std::string& texturePath)
	{
		m_pNormalTexture = Texture::LoadFromFile(texturePath);
	}

	void LambertShader::SetGlossTexture(const std::string& texturePath)
	{
		m_pGlossTexture = Texture::LoadFromFile(texturePath);
	}

	void LambertShader::SetSpecularTexture(const std::string& texturePath)
	{
		m_pSpecularTexture = Texture::LoadFromFile(texturePath);
	}

	void LambertShader::SetAmbientLight(const ColorRGB& color)
	{
		m_AmbientLight = color;
	}

	void LambertShader::SetLightDirection(const Vector3& direction)
	{
		m_LightDirection = direction;
	}

	void LambertShader::SetDiffuseReflection(float kd)
	{
		m_DiffuseReflection = kd;
	}

	void LambertShader::SetShininess(float shininess)
	{
		m_Shininess = shininess;
	}

	void LambertShader::SetAlphaClipping(float clipping)
	{
		m_AlphaClipping = clipping;
	}

}
