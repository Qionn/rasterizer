#include "UnlitShader.h"
#include "DataTypes.h"

namespace dae
{
	bool UnlitShader::CanShade(Vertex_Out& vertex) const
	{
		if (m_AlphaClipping > 0.0f && m_pDiffuseTexture != nullptr)
		{
			return m_pDiffuseTexture->SampleAlpha(vertex.uv) > m_AlphaClipping;
		}

		return true;
	}

	ColorRGB UnlitShader::Shade(Vertex_Out& vertex) const
	{
		ColorRGB color = vertex.color;

		if (m_pDiffuseTexture != nullptr)
		{
			color = m_pDiffuseTexture->SampleColor(vertex.uv);
		}

		color.MaxToOne();

		return color;
	}

	void UnlitShader::SetDiffuseTexture(const std::string& texturePath)
	{
		m_pDiffuseTexture = Texture::LoadFromFile(texturePath);
	}

	void UnlitShader::SetAlphaClipping(float clipping)
	{
		m_AlphaClipping = clipping;
	}
}
