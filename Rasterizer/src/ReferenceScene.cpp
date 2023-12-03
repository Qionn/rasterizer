#include "ReferenceScene.h"
#include "OpaqueShader.h"
#include "Utils.h"

namespace dae
{
	void ReferenceScene::Initialize()
	{
		ShadableObject spaceScooter{};

		Utils::ParseOBJ("Resources/vehicle.obj", spaceScooter.mesh.vertices, spaceScooter.mesh.indices);
		spaceScooter.mesh.primitiveTopology = PrimitiveTopology::TriangleList;
		spaceScooter.mesh.worldMatrix = Matrix::CreateTranslation(0.0f, 0.0f, 50.0f);

		auto pShader = std::make_unique<OpaqueShader>();

		pShader->SetDiffuseTexture("Resources/vehicle_diffuse.png");
		pShader->SetNormalTexture("Resources/vehicle_normal.png");
		pShader->SetGlossTexture("Resources/vehicle_gloss.png");
		pShader->SetSpecularTexture("Resources/vehicle_specular.png");

		spaceScooter.pShader = std::move(pShader);

		m_pSpaceScooter = AddShadableObject(std::move(spaceScooter));
	}

	void ReferenceScene::Update(Timer* pTimer)
	{
		const float rotationSpeed = 1.0f;
		m_pSpaceScooter->mesh.worldMatrix = Matrix::CreateRotationY(rotationSpeed * pTimer->GetElapsed()) * m_pSpaceScooter->mesh.worldMatrix;
	}
}
