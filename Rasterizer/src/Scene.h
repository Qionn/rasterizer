#pragma once

#include <vector>

#include "Timer.h"
#include "ShadableObject.h"

namespace dae
{
	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		Scene(const Scene&)				= delete;
		Scene& operator=(const Scene&)	= delete;
		Scene(Scene&&)					= delete;
		Scene& operator=(Scene&&)		= delete;

		virtual void Initialize() = 0;
		virtual void Update(Timer* pTimer) = 0;

		ShadableObject* AddShadableObject(ShadableObject shadableObject);

		std::vector<ShadableObject>& GetShadableObjects();
		const std::vector<ShadableObject>& GetShadableObjects() const;

	private:
		std::vector<ShadableObject> m_ShadableObjects;
	};
}
