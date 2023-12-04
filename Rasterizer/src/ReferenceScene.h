#pragma once

#include "Scene.h"

namespace dae
{
	class ReferenceScene final : public Scene
	{
	public:
		ReferenceScene() = default;
		~ReferenceScene() = default;

		ReferenceScene(const ReferenceScene&)				= delete;
		ReferenceScene& operator=(const ReferenceScene&)	= delete;
		ReferenceScene(ReferenceScene&&)					= delete;
		ReferenceScene& operator=(ReferenceScene&&)			= delete;

		void Initialize(float aspectRatio) override;
		void Update(Timer* pTimer) override;

	private:
		ShadableObject* m_pSpaceScooter{ nullptr };
		ShadableObject* m_pFireParticle1{ nullptr };
		ShadableObject* m_pFireParticle2{ nullptr };
	};
}