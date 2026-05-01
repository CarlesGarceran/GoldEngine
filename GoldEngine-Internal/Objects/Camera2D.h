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
			Engine::EngineObjects::Camera::Update();
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
	};
}