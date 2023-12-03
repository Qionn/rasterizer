#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface)
		: m_pSurface{ pSurface }
		, m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{

	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	std::unique_ptr<Texture> Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* pSurface = IMG_Load(path.c_str());
		return std::unique_ptr<Texture>{ new Texture(pSurface) };
	}

	ColorRGB Texture::SampleColor(const Vector2& uv) const
	{
		float alpha;
		ColorRGB color{ colors::Black };

		SampleHelper(uv, color.r, color.g, color.b, alpha);

		return color;
	}

	float Texture::SampleGray(const Vector2& uv) const
	{
		float r, g, b, a;
		SampleHelper(uv, r, g, b, a);
		return r;
	}

	float Texture::SampleAlpha(const Vector2& uv) const
	{
		float r, g, b, a;
		SampleHelper(uv, r, g, b, a);
		return a;
	}

	Vector3 Texture::SampleNormal(const Vector2& uv) const
	{
		float r, g, b, a;
		if (!SampleHelper(uv, r, g, b, a)) return Vector3::UnitZ;

		return {
			2.0f * r - 1.0f,
			2.0f * g - 1.0f,
			2.0f * b - 1.0f
		};
	}

	bool Texture::SampleHelper(const Vector2& uv, float& r, float& g, float& b, float& a) const
	{
		r = g = b = a = 0.0f;

		if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) return false;

		auto px = static_cast<Uint32>(uv.x * m_pSurface->w);
		auto py = static_cast<Uint32>(uv.y * m_pSurface->h);

		Uint8 red, green, blue, alpha;
		SDL_GetRGBA(m_pSurfacePixels[px + py * m_pSurface->w], m_pSurface->format, &red, &green, &blue, &alpha);

		r = red / 255.0f;
		g = green / 255.0f;
		b = blue / 255.0f;
		a = alpha / 255.0f;

		return true;
	}
}