#pragma once

#include "ColorRGB.h"

namespace dae
{
	class Texture;
	struct Vertex_Out;

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		Shader(const Shader&)				= delete;
		Shader& operator=(const Shader&)	= delete;
		Shader(Shader&&)					= delete;
		Shader& operator=(Shader&&)			= delete;

		virtual ColorRGB Shade(Vertex_Out& vertex) const = 0;
	};
}
