#pragma once

#include <memory>

#include "DataTypes.h"
#include "Shader.h"

namespace dae
{
	struct ShadableObject
	{
		Mesh mesh;
		std::unique_ptr<Shader> pShader;
	};
}
