#pragma once

#if !defined(PRODUCTION_BUILD)

namespace Engine::EngineObjects::Editor
{
	public ref class EditorCamera : public Engine::EngineObjects::Camera3D
	{
	public:
		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			this->transform->position = Engine::Components::Vector3::create(((Native::NativeCamera3D*)this->get())->get().position);
			nativeCamera->get().fovy = fov;
			transform->rotation = Engine::Components::Vector3::create(((Native::NativeCamera3D*)this->get())->get().target);

			if (cameraMode == CamMode::CAMERA_CUSTOM)
			{
				this->nativeCamera->get().target = transform->rotation.toNative();
				this->nativeCamera->getCameraPtr()->position = transform->position.toNative();
			}

			UpdateCamera(this->nativeCamera->getCameraPtr(), (int)cameraMode);
		}


		void OnActive() override
		{
			this->nativeCamera->getCameraPtr()->position = transform->position.toNative();
		}
	};
}

#endif