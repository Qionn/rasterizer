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
		void OnEvent(const SDL_Event& e) override;

	private:
		ShadableObject* m_pSpaceScooter{ nullptr };

		bool m_DebugRotate{};
	};
}
