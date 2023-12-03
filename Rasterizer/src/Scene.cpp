#include "Scene.h"

#include <cassert>

namespace dae
{
	void Scene::Initialize(float aspectRatio)
	{
		m_Camera.Initialize(aspectRatio);
	}

	void Scene::Update(Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}

	ShadableObject* Scene::AddShadableObject(ShadableObject shadableObject)
	{
		m_ShadableObjects.emplace_back(std::move(shadableObject));
		return &m_ShadableObjects.back();
	}

	std::list<ShadableObject>& Scene::GetShadableObjects()
	{
		return m_ShadableObjects;
	}

	const std::list<ShadableObject>& Scene::GetShadableObjects() const
	{
		return m_ShadableObjects;
	}

	Camera& Scene::GetCamera()
	{
		return m_Camera;
	}

	const Camera& Scene::GetCamera() const
	{
		return m_Camera;
	}

}
