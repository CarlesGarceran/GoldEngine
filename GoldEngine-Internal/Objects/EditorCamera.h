#pragma once

#if !defined(PRODUCTION_BUILD)

namespace Engine::EngineObjects::Editor
{
	public ref class EditorCamera : public Engine::EngineObjects::Camera3D
	{
	public:
		EditorCamera(String^ name, Engine::Internal::Components::Transform^ trans) 
			: Engine::EngineObjects::Camera3D(name, trans)
		{

		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			nativeCamera->get().fovy = fov;

			if (cameraMode == CamMode::CAMERA_FREE)
			{
				// Native camera is authoritative
				transform->position =
					Engine::Components::Vector3::create(nativeCamera->get().position);

				Engine::Components::Vector3 forward =
					Engine::Components::Vector3::create(
						RAYMATH::Vector3Subtract(nativeCamera->get().target, nativeCamera->get().position)
					).Normalized();

				transform->rotation =
					Engine::Components::Quaternion::LookRotation(forward, Engine::Components::Vector3(0,1,0));

				return; // IMPORTANT: do NOT overwrite native camera
			}

			UpdateCamera(nativeCamera->getCameraPtr(), (int)cameraMode);
		}

		void OnActive() override
		{
			this->nativeCamera->getCameraPtr()->position = transform->position.toNative();
		}
	};
}

#endif