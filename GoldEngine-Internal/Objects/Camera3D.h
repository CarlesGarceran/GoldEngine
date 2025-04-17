
namespace Engine::EngineObjects
{
	public ref class Camera3D : public Engine::EngineObjects::Camera
	{
	protected: 
		Native::NativeCamera3D* nativeCamera;

	public:
		Camera3D(String^ name, Engine::Internal::Components::Transform^ trans) : Engine::EngineObjects::Camera(name, trans, CameraProjection::CAMERA_PERSPECTIVE)
		{
			nativeCamera = new Native::NativeCamera3D((RAYLIB::CameraProjection)((int)cameraProjection));

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
			Engine::Components::Vector3^ fwd = gcnew Engine::Components::Vector3(0,0,0);
			fwd->copy(transform->forward);
			DrawLine3D(transform->position->toNative(), nativeCamera->get().target, GetColor(0xFF0000FF));
		}

		void setTarget(Engine::Components::Vector2^ target) override
		{
			auto nativeCameraPtr = this->nativeCamera->getCameraPtr();

			nativeCameraPtr->target.x = nativeCameraPtr->position.x + cosf(DEG2RAD * target->x) * cosf(DEG2RAD * target->y);
			nativeCameraPtr->target.y = nativeCameraPtr->position.y + sinf(DEG2RAD * target->y);
			nativeCameraPtr->target.z = nativeCameraPtr->position.z + sinf(DEG2RAD * target->x) * cosf(DEG2RAD * target->y);

			attributes->getAttribute("camera direction")->setValue(gcnew Engine::Components::Vector3(nativeCameraPtr->target.x, nativeCameraPtr->target.y, nativeCameraPtr->target.z));
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
			return true;
		}

		void LookAt(Engine::Components::Vector3^ target)
		{
			attributes->getAttribute("camera direction")->setValue(target);

			nativeCamera->get().target = target->toNative();

			RAYLIB::Vector3 forward = RAYMATH::Vector3Normalize(RAYMATH::Vector3Subtract(target->toNative(), nativeCamera->get().position));
			RAYLIB::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };

			RAYLIB::Vector3 right = RAYMATH::Vector3Normalize(RAYMATH::Vector3CrossProduct(worldUp, forward));
			RAYLIB::Vector3 up = RAYMATH::Vector3CrossProduct(forward, right);

			nativeCamera->get().up = up;
		}

		void LookAt(GameObject^ instance)
		{
			LookAt(instance->transform->position);
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