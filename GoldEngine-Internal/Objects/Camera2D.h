#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Camera2D : public Engine::EngineObjects::Camera
	{
	public:
		Camera2D(String^ name, Engine::Internal::Components::Transform^ trans) : Engine::EngineObjects::Camera(name, trans, CameraProjection::CAMERA_ORTHOGRAPHIC)
		{
			nativeCamera = new Native::NativeCamera3D((RAYLIB::CameraProjection)cameraProjection);
		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			nativeCamera->getCameraPtr()->fovy = fov;

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