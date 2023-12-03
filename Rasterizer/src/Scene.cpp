#include "Scene.h"

#include <cassert>

namespace dae
{
	void Scene::Initialize()
	{

	}

	ShadableObject* Scene::AddShadableObject(ShadableObject shadableObject)
	{
		m_ShadableObjects.emplace_back(std::move(shadableObject));
		return &m_ShadableObjects.back();
	}

	std::vector<ShadableObject>& Scene::GetShadableObjects()
	{
		return m_ShadableObjects;
	}

	const std::vector<ShadableObject>& Scene::GetShadableObjects() const
	{
		return m_ShadableObjects;
	}

}
