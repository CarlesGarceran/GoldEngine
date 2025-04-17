#pragma once

namespace Engine::EngineObjects
{
	public ref class Camera2D : public Engine::EngineObjects::Camera
	{
	protected:
		Native::NativeCamera3D* nativeCamera;

	public:
		Camera2D(String^ name, Engine::Internal::Components::Transform^ trans) : Engine::EngineObjects::Camera(name, trans, CameraProjection::CAMERA_ORTHOGRAPHIC)
		{
			nativeCamera = new Native::NativeCamera3D((RAYLIB::CameraProjection)cameraProjection);

			if (!attributes->getAttribute("camera direction"))
			{
				attributes->addAttribute("camera direction", gcnew Engine::Components::Vector3(0, 0, 1));
			}
		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			if (!attributes->getAttribute("camera direction"))
			{
				attributes->addAttribute("camera direction", gcnew Engine::Components::Vector3(0, 0, 1));
			}

			if (cameraMode == CamMode::CAMERA_CUSTOM)
			{
				if (attributes->getAttribute("camera direction"))
					nativeCamera->get().target = ((Engine::Components::Vector3^)attributes->getAttribute("camera direction")->getValue())->toNative();

				nativeCamera->getCameraPtr()->position = transform->position->toNative();
			}

			UpdateCamera(nativeCamera->getCameraPtr(), (int)cameraMode);
		}

		void DrawGizmo() override
		{
			Engine::Components::Vector3^ fwd = gcnew Engine::Components::Vector3(0, 0, 0);
			fwd->copy(transform->forward);
			DrawLine3D(transform->position->toNative(), nativeCamera->get().target, GetColor(0xFF0000FF));
		}

		void setTarget(Engine::Components::Vector3^ target) override
		{
			attributes->getAttribute("camera direction")->setValue(target);
			this->nativeCamera->setCameraTarget(target->toNative());
		}

		void* get() override
		{
			return (void*)nativeCamera;
		}

		bool is3DCamera() override
		{
			return false;
		}

		void ApplyCameraYaw(float yaw, bool local) override
		{
			RAYLIB::CameraYaw(nativeCamera->getCameraPtr(), yaw, local);
			RAYLIB::Vector3 v3 = nativeCamera->get().target;
			attributes->getAttribute("camera direction")->setValue(gcnew Engine::Components::Vector3(v3.x, v3.y, v3.z));
		}

		void ApplyCameraPitch(float yaw) override
		{
			RAYLIB::CameraPitch(nativeCamera->getCameraPtr(), yaw);
			RAYLIB::Vector3 v3 = nativeCamera->get().target;
			attributes->getAttribute("camera direction")->setValue(gcnew Engine::Components::Vector3(v3.x, v3.y, v3.z));
		}

		void ApplyCameraRoll(float roll) override
		{
			RAYLIB::CameraRoll(nativeCamera->getCameraPtr(), roll);
			RAYLIB::Vector3 v3 = nativeCamera->get().target;
			attributes->getAttribute("camera direction")->setValue(gcnew Engine::Components::Vector3(v3.x, v3.y, v3.z));
		}
	};
}