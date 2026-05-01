#include <../SDK.h>
#include "Rig.h"
#include "Bone.h"

#include "../Abstract/Renderer.h"
#include "../imnotifications/ImNotifyWrapper.h"

using Renderer = Engine::EngineObjects::Geometry::Abstract::Renderer^;

inline static Engine::Components::Quaternion ToModelSpace(Engine::Components::Quaternion modelSpace, Engine::Components::Quaternion offset)
{
	return modelSpace.Inverse() * offset;
}

Engine::EngineObjects::Rigging::Rig::Rig()
{
	boneColor = gcnew Engine::Components::Color(255, 0, 0, 255);
}

void Engine::EngineObjects::Rigging::Rig::Awake()
{
	if (Parent == nullptr) Destroy(this);
	if (!Parent->IsA<Renderer>()) Destroy(this);

	Renderer renderer = Parent->As<Renderer>();

	model = renderer->GetModelPtr();
	bones = gcnew System::Collections::Generic::List<Bone^>();
	boneMap = gcnew System::Collections::Generic::Dictionary<String^, Bone^>();

	ImGuiNET::ImNotify::ImNotification notification(
		ImGuiNET::ImNotify::ImGuiToastType::ImGuiToastType_Warning, 
		5000, 
		"The Rig type is currently in development and is experimental, use at your own risk", 
		"WARNING!"
	);
	ImGuiNET::ImNotify::ImNotify::InsertNotification(notification);

	if (!armatureCreated)
	{
		transform->localPosition = Engine::Components::Vector3::Zero();

		if (GetModel().skeleton.boneCount > 0)
		{
			CreateArmature(GetModel());
		}
	}
	else
	{
		for each (GameObject^ descendant in GetDescendants())
		{
			if (descendant->IsA<Bone^>())
			{
				boneMap->TryAdd(descendant->name, descendant->As<Bone^>());
				bones->Add(descendant->As<Bone^>());
			}
		}
	}
}

void Engine::EngineObjects::Rigging::Rig::Update()
{
	auto model = GetModel();

	std::vector<RAYLIB::Transform> bonePositions = {};
	bonePositions.reserve(model.skeleton.boneCount);

	for (int boneId = 0; boneId < model.skeleton.boneCount; boneId++)
	{
		Bone^ bone = bones[boneId];

		Engine::Components::Vector3 modelPos = transform->InverseTransformPoint(bone->transform->position);

		RAYLIB::Vector3 pos = (modelPos).toNative();
		RAYLIB::Quaternion rot = ((transform->rotation.Inverse() * bone->transform->rotation).Normalized()).toNative();
		RAYLIB::Vector3 scl = (bone->transform->scale / transform->scale).toNative();

		bonePositions.push_back(
			{
				pos,
				rot,
				scl
			}
		);
	}

	RAYLIB::Transform* frames[1] = { bonePositions.data() };

	RAYLIB::ModelAnimation modelAnimation{};
	modelAnimation.boneCount = model.skeleton.boneCount;
	modelAnimation.keyframeCount = 1;
	modelAnimation.keyframePoses = frames;
	strcpy(modelAnimation.name, "BINDPOSE");

	if (!IsModelAnimationValid(model, modelAnimation)) 
	{ 
		printError("CORRUPTED ANIMATION"); 
		return; 
	}

	if (useGpuSkinning)
	{
		RAYLIB::UpdateModelAnimationBones(
			model,
			modelAnimation,
			0
		);
	}
	else
	{
		RAYLIB::UpdateModelAnimation(
			model,
			modelAnimation,
			0
		);
	}
}

void Engine::EngineObjects::Rigging::Rig::DrawGizmo()
{
	for each (Bone^ bone in bones)
	{
		Bone^ parent = nullptr;
		if ((parent = GetParentBone(bone)) == nullptr)
		{
			RAYLIB::DrawLine3D(
				bone->transform->position.toNative(),
				transform->position.toNative(),
				boneColor->toNative()
			);
		}
		else
		{
			RAYLIB::DrawLine3D(
				bone->transform->position.toNative(),
				parent->transform->position.toNative(),
				boneColor->toNative()
			);
		}
	}
}

void Engine::EngineObjects::Rigging::Rig::Destroy()
{
	this->model = nullptr;
	for each (Bone^ bone in bones)
	{
		bone->Destroy();
	}

	this->bones->Clear();
	this->boneMap->Clear();
}

Engine::EngineObjects::Rigging::Bone^ Engine::EngineObjects::Rigging::Rig::GetBone(String^ name)
{
	Bone^ bone;
	if (boneMap->TryGetValue(name, bone)) return bone;
	return nullptr;
}

Engine::EngineObjects::Rigging::Bone^ Engine::EngineObjects::Rigging::Rig::GetBone(int index)
{
	return bones[index];
}

Engine::EngineObjects::Rigging::Bone^ Engine::EngineObjects::Rigging::Rig::GetParentBone(Engine::EngineObjects::Rigging::Bone^ bone)
{
	if (bone->Parent->IsA<Bone^>())
		return bone->Parent->As<Bone^>();
	else
		return nullptr;
}

void Engine::EngineObjects::Rigging::Rig::CreateArmature(RAYLIB::Model& model)
{
	for (int i = 0; i < model.skeleton.boneCount; i++)
	{
		RAYLIB::BoneInfo boneInfo = model.skeleton.bones[i];
		RAYLIB::Transform t = model.skeleton.bindPose[i];

		Bone^ bone = gcnew Bone(this);
		bone->name = gcnew String(boneInfo.name);
		bone = (Bone^)InstantiateChild(bone);

		bone->transform->localScale = Engine::Components::Vector3::One;
		bone->transform->localRotation = Engine::Components::Quaternion(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w);
		bone->transform->localPosition = Engine::Components::Vector3(t.translation.x, t.translation.y, t.translation.z);

		bones->Add(bone);
		boneMap[bone->name] = bone;
	}

	for (int i = 0; i < model.skeleton.boneCount; i++)
	{
		RAYLIB::BoneInfo boneInfo = model.skeleton.bones[i];
		Bone^ bone = boneMap[gcnew String(boneInfo.name)];
		if (boneInfo.parent != -1)
		{
			bone->Parent = bones[boneInfo.parent];
		}
	}

	armatureCreated = true;
}
