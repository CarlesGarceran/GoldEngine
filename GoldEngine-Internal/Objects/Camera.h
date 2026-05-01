#pragma once

#include <memory>

using namespace Engine::Internal::Components;

namespace Engine::EngineObjects
{
	namespace Native
	{
		public class NativeCamera3D
		{
		private:
			RAYLIB::Camera camera;

		public:
			RAYLIB::Camera* getCameraPtr()
			{
				return &this->camera;
			}

			RAYLIB::Camera& get()
			{
				return this->camera;
			}

		public:
			NativeCamera3D(CameraProjection projection)
			{
				printf("Creating camera\n");
				this->camera = RAYLIB::Camera();
				this->camera.projection = projection;
				this->camera.fovy = 90;
				this->camera.position = { 0,0,0 };
				this->camera.target = { 0, 0, 1 };
				this->camera.up = { 0, 1, 0 };
			}

		public:
			void setCameraPosition(RAYLIB::Vector3 nativeVector)
			{
				camera.position = nativeVector;
			}
			
			void setCameraTarget(RAYLIB::Vector3 nativeVector)
			{
				camera.target = nativeVector;
			}

		};
	}

	public enum class CamProjection
	{
		CAMERA_PERSPECTIVE = 0,         // Perspective projection
		CAMERA_ORTHOGRAPHIC             // Orthographic projection
	};

	public enum class CamMode
	{
		CAMERA_CUSTOM = 0,              // Camera custom, controlled by user (UpdateCamera() does nothing)
		CAMERA_FREE,                    // Camera free mode
		CAMERA_ORBITAL,                 // Camera orbital, around target, zoom supported
		CAMERA_FIRST_PERSON,            // Camera first person
		CAMERA_THIRD_PERSON             // Camera third person
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Camera abstract : public Engine::EngineObjects::ScriptBehaviour
	{
	protected:
		Native::NativeCamera3D* nativeCamera;
		CamProjection cameraProjection;

	public:
		[Engine::Scripting::PropertyAttribute]
		CamMode cameraMode;

		[Engine::Scripting::PropertyAttribute]
		float fov = 60;

		[Engine::Scripting::PropertyAttribute]
		float nearPlane = 1.0f;

		[Engine::Scripting::PropertyAttribute]
		float farPlane = 10.0f;

		[Engine::Scripting::PropertyAttribute]
		bool IsMainCamera;
	public:
		Camera(String^ name, Engine::Internal::Components::Transform^ trans, int projection);

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;
		void DrawGizmo() override;

	public:
		CamProjection getProjection() { return cameraProjection; }

	public:
		virtual bool is3DCamera() abstract;
		virtual void* get() abstract;

		void setTarget(Engine::Components::Vector2 target) override;
		void setTarget(Engine::Components::Vector3 target);

		void LookAt(Engine::Components::Vector3 target);

		void LookAt(GameObject^ instance);

		void ApplyCameraYaw(float yaw, bool local);
		void ApplyCameraPitch(float pitch);
		void ApplyCameraRoll(float roll);

		static property Engine::EngineObjects::Camera^ Main
		{
			Engine::EngineObjects::Camera^ get();
		}
	};
}