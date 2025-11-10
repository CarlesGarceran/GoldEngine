#pragma once

namespace Engine::Bridge
{
	public ref class UnsafeModelAPI
	{
	public:
		static RAYLIB::Mesh** GetMeshes(RAYLIB::Model* model);
		static RAYLIB::BoneInfo** GetBones(RAYLIB::Model* model);

		static RAYLIB::Mesh* GetMesh(RAYLIB::Model* model, int meshIndex);
		static RAYLIB::BoneInfo* GetBone(RAYLIB::Model* model, int boneIndex);

		static Engine::Components::Matrix16^ GetTransform(RAYLIB::Model* model);

		static int GetMeshCount(RAYLIB::Model* model);
		static int GetBoneCount(RAYLIB::Model* model);

		static bool IsMesh(RAYLIB::Model* model);
	};
}