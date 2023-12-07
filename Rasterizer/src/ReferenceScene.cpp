#include <SDL.h>

#include "ReferenceScene.h"
#include "LambertShader.h"
#include "Utils.h"

namespace dae
{
	void ReferenceScene::Initialize(float aspectRatio)
	{
		// Initialize camera
		Camera& camera = GetCamera();
		camera.Initialize(aspectRatio, 0.1f, 100.0f, 45.0f, { 0.0f, 5.0f, -64.0f });
		camera.walkSpeed = 30.0f;

		// Create space scooter
		ShadableObject spaceScooter{};

		Utils::ParseOBJ("Resources/vehicle.obj", spaceScooter.mesh.vertices, spaceScooter.mesh.indices);
		spaceScooter.mesh.primitiveTopology = PrimitiveTopology::TriangleList;

		auto pLitShader = std::make_shared<LambertShader>();

		pLitShader->SetDiffuseTexture("Resources/vehicle_diffuse.png");
		pLitShader->SetNormalTexture("Resources/vehicle_normal.png");
		pLitShader->SetGlossTexture("Resources/vehicle_gloss.png");
		pLitShader->SetSpecularTexture("Resources/vehicle_specular.png");
		pLitShader->SetAmbientLight({ 0.3f, 0.3f, 0.3f });

		spaceScooter.pShader = pLitShader;

		m_pSpaceScooter = AddShadableObject(spaceScooter);
	}

	void ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		if (m_DebugRotate)
		{
			const float rotationSpeed = 1.0f;
			m_pSpaceScooter->mesh.worldMatrix = Matrix::CreateRotationY(rotationSpeed * pTimer->GetElapsed()) * m_pSpaceScooter->mesh.worldMatrix;
		}
	}

	void ReferenceScene::OnEvent(const SDL_Event& e)
	{
		if (e.type != SDL_KEYUP) return;

		switch (e.key.keysym.scancode)
		{
			case SDL_SCANCODE_F5:
				m_DebugRotate = !m_DebugRotate;
				break;
		}
	}

}
