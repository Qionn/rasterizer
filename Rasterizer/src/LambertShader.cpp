#include "LambertShader.h"

#include "DataTypes.h"

namespace dae
{
	bool LambertShader::s_EnableNormalMapping{ false };
	LambertShader::Mode LambertShader::s_Mode{ Mode::Combined };

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

		if (s_EnableNormalMapping && m_pNormalTexture != nullptr)
		{
			Vector3 binoral = Vector3::Cross(vertex.normal, vertex.tangent);
			Matrix tangentSpaceMatrix = Matrix{ vertex.tangent, binoral, vertex.normal, Vector3::Zero };
			normal = tangentSpaceMatrix.TransformVector(m_pNormalTexture->SampleNormal(vertex.uv));
		}

		// Diffuse color (lambert)
		ColorRGB color = colors::Black;

		if (m_pDiffuseTexture != nullptr)
		{
			color = m_pDiffuseTexture->SampleColor(vertex.uv);
		}
		else
		{
			color = vertex.color;
		}

		float lambertian = std::max(Vector3::Dot(-m_LightDirection, normal), 0.0f);
		ColorRGB lambertianRGB{ lambertian, lambertian, lambertian };

		float glossSample = (m_pGlossTexture != nullptr) ? m_pGlossTexture->SampleGray(vertex.uv) : 0.0f;
		float specularSample = (m_pSpecularTexture != nullptr) ? m_pSpecularTexture->SampleGray(vertex.uv) : 0.0f;

		ColorRGB lambert = LambertBRDF(color);
		ColorRGB specular = SpecularBRDF(glossSample, specularSample, m_LightDirection, vertex.viewDirection, normal);

		switch (s_Mode)
		{
			case Mode::ObservedArea:
				color = lambertianRGB;
				break;

			case Mode::Diffuse:
				color = lambert * lambertianRGB;
				break;

			case Mode::Specular:
				color = specular * lambertianRGB;
				break;

			case Mode::Combined:
				color = (lambert + specular) * (lambertianRGB + m_AmbientLight);
				break;
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

	void LambertShader::ToggleNormalMapping()
	{
		s_EnableNormalMapping = !s_EnableNormalMapping;
	}

	void LambertShader::CycleMode()
	{
		switch (s_Mode)
		{
			case Mode::ObservedArea:
				s_Mode = Mode::Diffuse;
				break;

			case Mode::Diffuse:
				s_Mode = Mode::Specular;
				break;

			case Mode::Specular:
				s_Mode = Mode::Combined;
				break;

			case Mode::Combined:
				s_Mode = Mode::ObservedArea;
				break;
		}
	}

	ColorRGB LambertShader::LambertBRDF(const ColorRGB& cd) const
	{
		return cd * m_DiffuseReflection / PI;
	}

	ColorRGB LambertShader::SpecularBRDF(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n) const
	{
		float cosa = Vector3::Dot(Vector3::Reflect(-m_LightDirection, n), v);

		if (cosa <= 0.0f)
		{
			return colors::Black;
		}

		float phong = ks * std::pow(cosa, exp * m_Shininess);
		return ColorRGB{ phong, phong, phong };
	}

}
