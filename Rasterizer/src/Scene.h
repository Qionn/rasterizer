#pragma once

#include <vector>

#include "Camera.h"
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

		virtual void Initialize(float aspectRatio);
		virtual void Update(Timer* pTimer);

		ShadableObject* AddShadableObject(ShadableObject shadableObject);

		std::vector<ShadableObject>& GetShadableObjects();
		const std::vector<ShadableObject>& GetShadableObjects() const;

		Camera& GetCamera();
		const Camera& GetCamera() const;

	private:
		std::vector<ShadableObject> m_ShadableObjects;
		Camera m_Camera;
	};
}
