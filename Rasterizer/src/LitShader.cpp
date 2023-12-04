#include "LitShader.h"

#include "DataTypes.h"

namespace dae
{
	bool LitShader::CanShade(Vertex_Out& vertex) const
	{
		if (m_AlphaClipping > 0.0f && m_pDiffuseTexture != nullptr)
		{
			return m_pDiffuseTexture->SampleAlpha(vertex.uv) > m_AlphaClipping;
		}

		return true;
	}

	ColorRGB LitShader::Shade(Vertex_Out& vertex) const
	{
		Vector3 normal = vertex.normal;

		if (m_pNormalTexture != nullptr)
		{
			Vector3 binoral = Vector3::Cross(vertex.normal, vertex.tangent);
			Matrix tangentSpaceMatrix = Matrix{ vertex.tangent, binoral, vertex.normal, Vector3::Zero };
			normal = tangentSpaceMatrix.TransformVector(m_pNormalTexture->SampleNormal(vertex.uv));
		}

		ColorRGB color = vertex.color;

		if (m_pDiffuseTexture != nullptr)
		{
			color = m_pDiffuseTexture->SampleColor(vertex.uv);
		}

		if (m_pGlossTexture != nullptr && m_pSpecularTexture != nullptr)
		{
			float cosa = Vector3::Dot(Vector3::Reflect(-m_LightDirection, normal), vertex.viewDirection);
			if (cosa > 0.0f)
			{
				float phong = m_pGlossTexture->SampleGray(vertex.uv) * std::pow(cosa, m_pSpecularTexture->SampleGray(vertex.uv) * m_Shininess);
				color += ColorRGB{ phong, phong, phong };
			}
		}

		// Observed Area
		color *= std::max(Vector3::Dot(-m_LightDirection, normal), 0.0f);
		color.MaxToOne();

		return color;
	}

	void LitShader::SetDiffuseTexture(const std::string& texturePath)
	{
		m_pDiffuseTexture = Texture::LoadFromFile(texturePath);
	}

	void LitShader::SetNormalTexture(const std::string& texturePath)
	{
		m_pNormalTexture = Texture::LoadFromFile(texturePath);
	}

	void LitShader::SetGlossTexture(const std::string& texturePath)
	{
		m_pGlossTexture = Texture::LoadFromFile(texturePath);
	}

	void LitShader::SetSpecularTexture(const std::string& texturePath)
	{
		m_pSpecularTexture = Texture::LoadFromFile(texturePath);
	}

	void LitShader::SetLightDirection(const Vector3& direction)
	{
		m_LightDirection = direction;
	}

	void LitShader::SetShininess(float shininess)
	{
		m_Shininess = shininess;
	}

	void LitShader::SetAlphaClipping(float clipping)
	{
		m_AlphaClipping = clipping;
	}

}
