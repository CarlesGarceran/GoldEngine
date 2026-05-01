#include <../SDK.h>
#include "Bone.h"
#include "Rig.h"

Engine::EngineObjects::Rigging::Bone::Bone() :
	Engine::EngineObjects::Script(),
	rig(nullptr)
{

}

void Engine::EngineObjects::Rigging::Bone::DrawGizmo()
{
	RAYLIB::DrawCube(
		transform->position.toNative(),
		0.01f, 0.01f, 0.01f,
		rig->As<Rig^>()->boneColor->toNative()
	);
}

void Engine::EngineObjects::Rigging::Bone::Destroy()
{
	this->rig = nullptr;
}

Engine::EngineObjects::Rigging::Rig^ Engine::EngineObjects::Rigging::Bone::GetRig()
{
	return rig->As<Engine::EngineObjects::Rigging::Rig^>();
}

Engine::EngineObjects::Rigging::Bone::Bone(Engine::EngineObjects::Rigging::Rig^ rig) :
	Engine::EngineObjects::Script(),
	rig(rig)
{

}
