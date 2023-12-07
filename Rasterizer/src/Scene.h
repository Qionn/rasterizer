#pragma once

#include <list>

#include "Camera.h"
#include "Timer.h"
#include "ShadableObject.h"

union SDL_Event;

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
		virtual void OnEvent(const SDL_Event& e);

		ShadableObject* AddShadableObject(ShadableObject shadableObject);

		std::list<ShadableObject>& GetShadableObjects();
		const std::list<ShadableObject>& GetShadableObjects() const;

		Camera& GetCamera();
		const Camera& GetCamera() const;

	private:
		std::list<ShadableObject> m_ShadableObjects;
		Camera m_Camera;
	};
}
