#include "../SDK.h"
#include "NAPIBridge.h"

using namespace Engine::Components;

RAYLIB::Mesh** Engine::Bridge::UnsafeModelAPI::GetMeshes(RAYLIB::Model* model)
{
	return &model->meshes;
}

RAYLIB::BoneInfo** Engine::Bridge::UnsafeModelAPI::GetBones(RAYLIB::Model* model)
{
	return &model->bones;
}

RAYLIB::Mesh* Engine::Bridge::UnsafeModelAPI::GetMesh(RAYLIB::Model* model, int meshIndex)
{
	return &model->meshes[meshIndex];
}

RAYLIB::BoneInfo* Engine::Bridge::UnsafeModelAPI::GetBone(RAYLIB::Model* model, int boneIndex)
{
	return &model->bones[boneIndex];
}

int Engine::Bridge::UnsafeModelAPI::GetMeshCount(RAYLIB::Model* model)
{
	return model->meshCount;
}

int Engine::Bridge::UnsafeModelAPI::GetBoneCount(RAYLIB::Model* model)
{
	return model->boneCount;
}

Engine::Components::Matrix16^ Engine::Bridge::UnsafeModelAPI::GetTransform(RAYLIB::Model* model)
{
	return gcnew Matrix16(model->transform);
}

bool Engine::Bridge::UnsafeModelAPI::IsMesh(RAYLIB::Model* model)
{
	if (model == nullptr) return false;

	return RAYLIB::IsModelValid(*model);
}
