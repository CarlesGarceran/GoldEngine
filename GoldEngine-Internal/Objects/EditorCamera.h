#pragma once

#if !defined(PRODUCTION_BUILD)

namespace Engine::EngineObjects::Editor
{
	public ref class EditorCamera : public Engine::EngineObjects::Camera3D
	{
	public:
		EditorCamera(String^ name, Engine::Internal::Components::Transform^ parent) : Engine::EngineObjects::Camera3D(name, parent)
		{

		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			this->transform->position = Engine::Components::Vector3::create(((Native::NativeCamera3D*)this->get())->get().position);
			nativeCamera->get().fovy = fov;
			
			if (attributes->hasAttribute("camera direction"))
			{
				attributes->getAttribute("camera direction")->setValue(Engine::Components::Vector3::create(((Native::NativeCamera3D*)this->get())->get().target));
			}

			if (cameraMode == CamMode::CAMERA_CUSTOM)
			{
				if (attributes->getAttribute("camera direction"))
					this->nativeCamera->get().target = ((Engine::Components::Vector3^)attributes->getAttribute("camera direction")->getValue())->toNative();

				this->nativeCamera->getCameraPtr()->position = transform->position->toNative();
			}

			UpdateCamera(this->nativeCamera->getCameraPtr(), (int)cameraMode);
		}


		void OnActive() override
		{
			this->nativeCamera->getCameraPtr()->position = transform->position->toNative();
		}
	};
}

#endif