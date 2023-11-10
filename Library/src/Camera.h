#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward	= Vector3::UnitZ;
		Vector3 up		= Vector3::UnitY;
		Vector3 right	= Vector3::UnitX;

		float totalPitch	= 0.0f;
		float totalYaw		= 0.0f;

		float rotationSpeed = 0.01f;
		float walkSpeed		= 10.0f;
		float dragSpeed		= 5.0f;

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			Matrix rotation = Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw);
			forward = rotation.GetAxisZ();
			right = rotation.GetAxisX();
			up = rotation.GetAxisY();

			invViewMatrix = Matrix::CreateLookAtLH(origin, forward, Vector3::UnitY);
			viewMatrix = invViewMatrix.Inverse();
		}

		void CalculateProjectionMatrix()
		{
			//TODO W3

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			HandleKeyboardInput(deltaTime);
			HandleMouseInput(deltaTime);

			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}

		void HandleKeyboardInput(float dt)
		{
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			const int8_t xDir = pKeyboardState[SDL_SCANCODE_D] - pKeyboardState[SDL_SCANCODE_A];
			const int8_t zDir = pKeyboardState[SDL_SCANCODE_W] - pKeyboardState[SDL_SCANCODE_S];

			origin += zDir * walkSpeed * forward * dt;
			origin += xDir * walkSpeed * right * dt;
		}

		void HandleMouseInput(float dt)
		{
			int mouseX, mouseY;
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool isLeftMouseDown = static_cast<bool>(mouseState & SDL_BUTTON(1));
			const bool isRightMouseDown = static_cast<bool>(mouseState & SDL_BUTTON(3));
			const bool areBothMouseDown = isLeftMouseDown && isRightMouseDown;

			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(isLeftMouseDown || isRightMouseDown));

			if (areBothMouseDown)
			{
				origin -= mouseY * dragSpeed * Vector3::UnitY * dt;
			}
			else if (isRightMouseDown)
			{
				totalYaw += mouseX * rotationSpeed;
				totalPitch -= mouseY * rotationSpeed;
			}
			else if (isLeftMouseDown)
			{
				origin -= mouseY * dragSpeed * forward * dt;
				totalYaw += mouseX * rotationSpeed;
			}
		}

	};
}
