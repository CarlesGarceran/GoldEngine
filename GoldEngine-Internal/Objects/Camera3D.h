
namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Camera3D : public Engine::EngineObjects::Camera
	{
	protected: 
		Native::NativeCamera3D* nativeCamera;

	public:
		Camera3D(String^ name, Engine::Internal::Components::Transform^ trans) : Engine::EngineObjects::Camera(name, trans, CameraProjection::CAMERA_PERSPECTIVE)
		{
			nativeCamera = new Native::NativeCamera3D((RAYLIB::CameraProjection)((int)cameraProjection));
		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			if (transform->rotation.Equals(Engine::Components::Vector3::Zero()))
				transform->rotation = Engine::Components::Vector3(0, 0, 1);

			nativeCamera->get().fovy = fov;

			if (cameraMode == CamMode::CAMERA_CUSTOM)
			{
				nativeCamera->get().target = transform->rotation.toNative();
				nativeCamera->getCameraPtr()->position = transform->position.toNative();
			}
			
			UpdateCamera(nativeCamera->getCameraPtr(), (int)cameraMode);
		}

		void DrawGizmo() override
		{
			Engine::Components::Vector3 fwd = transform->forward;
			DrawLine3D(transform->position.toNative(), nativeCamera->get().target, GetColor(0xFF0000FF));
		}

		void setTarget(Engine::Components::Vector2 target) override
		{
			auto nativeCameraPtr = this->nativeCamera->getCameraPtr();

			nativeCameraPtr->target.x = nativeCameraPtr->position.x + cosf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);
			nativeCameraPtr->target.y = nativeCameraPtr->position.y + sinf(DEG2RAD * target.y);
			nativeCameraPtr->target.z = nativeCameraPtr->position.z + sinf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);

			transform->rotation = Engine::Components::Vector3(nativeCameraPtr->target.x, nativeCameraPtr->target.y, nativeCameraPtr->target.z);
		}

		void setTarget(Engine::Components::Vector3 target) override
		{
			transform->rotation = target;
			this->nativeCamera->setCameraTarget(target.toNative());
		}

		void* get() override
		{
			return (void*)nativeCamera;
		}

		bool is3DCamera() override
		{
			return true;
		}

		void LookAt(Engine::Components::Vector3 target)
		{
			transform->rotation = target;

			nativeCamera->get().target = target.toNative();

			RAYLIB::Vector3 forward = RAYMATH::Vector3Normalize(RAYMATH::Vector3Subtract(target.toNative(), nativeCamera->get().position));
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
			transform->rotation = Engine::Components::Vector3(v3.x, v3.y, v3.z);
		}

		void ApplyCameraPitch(float yaw) override
		{
			RAYLIB::CameraPitch(nativeCamera->getCameraPtr(), yaw);
			RAYLIB::Vector3 v3 = nativeCamera->get().target;
			transform->rotation = Engine::Components::Vector3(v3.x, v3.y, v3.z);
		}

		void ApplyCameraRoll(float roll) override
		{
			RAYLIB::CameraRoll(nativeCamera->getCameraPtr(), roll);
			RAYLIB::Vector3 v3 = nativeCamera->get().target;
			transform->rotation = Engine::Components::Vector3(v3.x, v3.y, v3.z);
		}
	};
}