#include "../SDK.h"
#include "Camera.h"

Engine::EngineObjects::Camera::Camera(String^ name, Engine::Internal::Components::Transform^ trans, int projection)
	: Engine::EngineObjects::ScriptBehaviour(name, trans),
	cameraProjection((CamProjection)projection)
{
}

void Engine::EngineObjects::Camera::Update()
{
	nativeCamera->get().fovy = fov;

	if (cameraMode == CamMode::CAMERA_CUSTOM)
	{
		// Compute forward from rotation quaternion
		Engine::Components::Vector3 forward = Engine::Components::Quaternion::Rotate(transform->rotation, Engine::Components::Vector3(0, 0, 1));
		nativeCamera->get().target = (transform->position + forward).toNative();
		nativeCamera->getCameraPtr()->position = transform->position.toNative();

		// Rotate up vector
		nativeCamera->get().up = Engine::Components::Quaternion::Rotate(transform->rotation, Engine::Components::Vector3(0, 1, 0)).toNative();
	}

	if (cameraMode != CamMode::CAMERA_CUSTOM) UpdateCamera(nativeCamera->getCameraPtr(), (int)cameraMode);
}

void Engine::EngineObjects::Camera::DrawGizmo()
{
	DrawLine3D(
		transform->position.toNative(), 
		(transform->position + transform->forward).toNative(), 
		GetColor(0xFF0000FF)
	);
}

void Engine::EngineObjects::Camera::setTarget(Engine::Components::Vector2 target)
{
	float x = transform->position.x + cosf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);
	float y = transform->position.y + sinf(DEG2RAD * target.y);
	float z = transform->position.z + sinf(DEG2RAD * target.x) * cosf(DEG2RAD * target.y);

	LookAt(Engine::Components::Vector3(x, y, z));
}

void Engine::EngineObjects::Camera::setTarget(Engine::Components::Vector3 target)
{
	LookAt(target);
}

void Engine::EngineObjects::Camera::LookAt(Engine::Components::Vector3 target)
{
	Engine::Components::Vector3 forward = (target - transform->position).Normalized();
	Engine::Components::Vector3 worldUp = Engine::Components::Vector3(0, 1, 0);

	if (fabs(Engine::Components::Vector3::Dot(forward, worldUp)) > 0.999f)
		worldUp = Engine::Components::Vector3(0, 0, 1);

	transform->rotation = Engine::Components::Quaternion::LookRotation(forward, worldUp);
	nativeCamera->get().target = target.toNative();

	Engine::Components::Vector3 rotatedUp = Engine::Components::Quaternion::Rotate(transform->rotation, worldUp);
	nativeCamera->get().up = rotatedUp.toNative();
}

void Engine::EngineObjects::Camera::LookAt(GameObject^ instance)
{
	LookAt(instance->transform->position);
}

void Engine::EngineObjects::Camera::ApplyCameraYaw(float yaw, bool local)
{
	Engine::Components::Quaternion rot = Engine::Components::Quaternion::FromAxis(Engine::Components::Vector3(0, 1, 0), yaw * DEG2RAD);
	transform->rotation = local ? transform->rotation * rot : rot * transform->rotation;
}

void Engine::EngineObjects::Camera::ApplyCameraPitch(float pitch)
{
	Engine::Components::Quaternion rot = Engine::Components::Quaternion::FromAxis(Engine::Components::Vector3(1, 0, 0), pitch * DEG2RAD);
	transform->rotation = rot * transform->rotation;
}

void Engine::EngineObjects::Camera::ApplyCameraRoll(float roll)
{
	Engine::Components::Quaternion rot = Engine::Components::Quaternion::FromAxis(Engine::Components::Vector3(0, 0, 1), roll * DEG2RAD);
	transform->rotation = rot * transform->rotation;
}

Engine::EngineObjects::Camera^ Engine::EngineObjects::Camera::Main::get()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetMainCamera(EngineState::PlayMode);
}
