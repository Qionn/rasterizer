#pragma once

#include <memory>

#include "DataTypes.h"
#include "Shader.h"

namespace dae
{
	struct ShadableObject
	{
		Mesh mesh;
		std::shared_ptr<Shader> pShader;
	};
}
