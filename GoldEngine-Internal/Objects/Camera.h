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
		Camera(String^ name, Engine::Internal::Components::Transform^ trans, int projection) : Engine::EngineObjects::ScriptBehaviour(name, trans)
		{
			cameraProjection = (CamProjection)projection;
		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{

		}

		void DrawGizmo() override
		{
			DrawLine3D(transform->position.toNative(), transform->forward.toNative(), GetColor(0xFF0000FF));
		}

	public:
		CamProjection getProjection() { return cameraProjection; }

	public:
		virtual bool is3DCamera() abstract;
		virtual void* get() abstract;

		virtual void ApplyCameraYaw(float yaw, bool local) abstract;
		virtual void ApplyCameraPitch(float pitch) abstract;
		virtual void ApplyCameraRoll(float roll) abstract;

		void setTarget(Engine::Components::Vector2 target) override
		{
			float x = transform->position.x + cosf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);
			float y = transform->position.y + sinf(DEG2RAD * target.y);
			float z = transform->position.z + sinf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);

			LookAt(Engine::Components::Vector3(x, y, z));
		}

		void setTarget(Engine::Components::Vector3 target)
		{
			LookAt(target);
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
	};
}