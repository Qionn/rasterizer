#include "ReferenceScene.h"
#include "LitShader.h"
#include "UnlitShader.h"
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

		auto pLitShader = std::make_shared<LitShader>();

		pLitShader->SetDiffuseTexture("Resources/vehicle_diffuse.png");
		pLitShader->SetNormalTexture("Resources/vehicle_normal.png");
		pLitShader->SetGlossTexture("Resources/vehicle_gloss.png");
		pLitShader->SetSpecularTexture("Resources/vehicle_specular.png");

		spaceScooter.pShader = pLitShader;

		m_pSpaceScooter = AddShadableObject(spaceScooter);

		// Create fire particles
		ShadableObject fireParticle{};

		Utils::ParseOBJ("Resources/quad.obj", fireParticle.mesh.vertices, fireParticle.mesh.indices);
		fireParticle.mesh.primitiveTopology = PrimitiveTopology::TriangleList;
		fireParticle.mesh.worldMatrix = Matrix::CreateScale(5.0f, 5.0f, 5.0f) * Matrix::CreateTranslation(0.0f, 10.0f, 50.0f);

		auto pUnlitShader = std::make_shared<UnlitShader>();
		pUnlitShader->SetDiffuseTexture("Resources/fire_particle.png");
		pUnlitShader->SetAlphaClipping(0.05f);

		fireParticle.pShader = pUnlitShader;

		m_pFireParticle1 = AddShadableObject(fireParticle);
		m_pFireParticle2 = AddShadableObject(fireParticle);
	}

	void ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const float rotationSpeed = 1.0f;
		m_pSpaceScooter->mesh.worldMatrix = Matrix::CreateRotationY(rotationSpeed * pTimer->GetElapsed()) * m_pSpaceScooter->mesh.worldMatrix;

		const Vector3 particleOffset{ -21.0f, -1.0f, 0.0f };
		const Vector3 particleAngle{ 0.0f, 90.0f * TO_RADIANS, 120.0f * TO_RADIANS };

		m_pFireParticle1->mesh.worldMatrix = Matrix::CreateRotation(0.0f, 0.0f, particleAngle.z) * Matrix::CreateTranslation(particleOffset) * m_pSpaceScooter->mesh.worldMatrix;
		m_pFireParticle2->mesh.worldMatrix = Matrix::CreateRotation(particleAngle) * Matrix::CreateTranslation(particleOffset) * m_pSpaceScooter->mesh.worldMatrix;
	}
}
