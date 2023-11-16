#pragma once

#include "DataTypes.h"

namespace dae
{
	struct Triangle final
	{
		Vertex_Out v0, v1, v2;
		Vector2 e1, e2, e3;

		Triangle(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2)
			: v0{ v0 }
			, v1{ v1 }
			, v2{ v2 }
			, e1{ (v1.position - v0.position).GetXY() }
			, e2{ (v2.position - v1.position).GetXY() }
			, e3{ (v0.position - v2.position).GetXY() }
		{

		}
	};
}
