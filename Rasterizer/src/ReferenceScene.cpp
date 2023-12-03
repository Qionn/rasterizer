#include "ReferenceScene.h"
#include "OpaqueShader.h"
#include "Utils.h"

namespace dae
{
	void ReferenceScene::Initialize(float aspectRatio)
	{
		// Initialize camera
		Camera& camera = GetCamera();
		camera.Initialize(aspectRatio, 45.0f);
		camera.walkSpeed = 30.0f;

		// Create space scooter
		ShadableObject spaceScooter{};

		Utils::ParseOBJ("Resources/vehicle.obj", spaceScooter.mesh.vertices, spaceScooter.mesh.indices);
		spaceScooter.mesh.primitiveTopology = PrimitiveTopology::TriangleList;
		spaceScooter.mesh.worldMatrix = Matrix::CreateTranslation(0.0f, 0.0f, 50.0f);

		auto pShader = std::make_shared<OpaqueShader>();

		pShader->SetDiffuseTexture("Resources/vehicle_diffuse.png");
		pShader->SetNormalTexture("Resources/vehicle_normal.png");
		pShader->SetGlossTexture("Resources/vehicle_gloss.png");
		pShader->SetSpecularTexture("Resources/vehicle_specular.png");

		spaceScooter.pShader = pShader;

		m_pSpaceScooter = AddShadableObject(spaceScooter);
	}

	void ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const float rotationSpeed = 1.0f;
		m_pSpaceScooter->mesh.worldMatrix = Matrix::CreateRotationY(rotationSpeed * pTimer->GetElapsed()) * m_pSpaceScooter->mesh.worldMatrix;
	}
}
