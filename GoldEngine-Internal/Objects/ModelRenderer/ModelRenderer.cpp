#include "../../SDK.h"
#include "../Abstract/Renderer.h"
#include "ModelRenderer.hpp"

using namespace Engine::EngineObjects::Geometry;
using namespace Engine::Assets::Storage;
using namespace Engine::Native;

UNMANAGED_BEGIN

inline static RAYLIB::Model CopyModel(RAYLIB::Model& ref)
{
	RAYLIB::Model cpy = {};

	cpy.meshCount = ref.meshCount;
	cpy.materialCount = ref.materialCount;

	cpy.skeleton.boneCount = ref.skeleton.boneCount;
	cpy.boneMatrices = new RAYLIB::Matrix[ref.skeleton.boneCount];
	memcpy(cpy.boneMatrices, ref.boneMatrices, sizeof(RAYLIB::Matrix) * ref.skeleton.boneCount);

	cpy.currentPose = new RAYLIB::Transform[ref.skeleton.boneCount];

	for (int x = 0; x < ref.skeleton.boneCount; x++)
	{
		cpy.currentPose[x].translation = ref.currentPose[x].translation;
		cpy.currentPose[x].rotation = ref.currentPose[x].rotation;
		cpy.currentPose[x].scale = ref.currentPose[x].scale;
	}

	cpy.skeleton.bones = new RAYLIB::BoneInfo[ref.skeleton.boneCount];
	cpy.skeleton.bindPose = new RAYLIB::Transform[ref.skeleton.boneCount];
	
	for (int x = 0; x < ref.skeleton.boneCount; x++)
	{
		cpy.skeleton.bindPose[x].translation = ref.skeleton.bindPose[x].translation;
		cpy.skeleton.bindPose[x].rotation = ref.skeleton.bindPose[x].rotation;
		cpy.skeleton.bindPose[x].scale = ref.skeleton.bindPose[x].scale;

		cpy.skeleton.bones[x].parent = ref.skeleton.bones[x].parent;
		strcpy(cpy.skeleton.bones[x].name, ref.skeleton.bones[x].name);
	}

	cpy.meshes = ref.meshes;
	cpy.transform = ref.transform;
	cpy.materials = ref.materials;
	cpy.meshMaterial = ref.meshMaterial;

	return cpy;
}

inline void Deallocate(RAYLIB::Model& cpy)
{
	delete[] cpy.boneMatrices;
	delete[] cpy.currentPose;
	delete[] cpy.skeleton.bones;
	delete[] cpy.skeleton.bindPose;
}

UNMANAGED_END


Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer()
	: Engine::EngineObjects::Geometry::Abstract::Renderer()
{

}

void ModelRenderer::Awake()
{
	this->tint = Engine::Components::Color::New();
	RAYLIB::Model model = DataPacks::singleton().GetModel(modelId);
	this->modelPtr = new EnginePtr<RAYLIB::Model>(CopyModel(model), &Deallocate, &Deallocate);

	GetPropertyChangedEvent("modelId")->Connect(gcnew System::Action<unsigned int, unsigned int>(this, &ModelRenderer::OnModelIdChanged));
}

void ModelRenderer::Draw()
{
	RAYLIB::Model model = this->modelPtr->getInstance();

	Engine::Components::Material^ material = DataPacks::singleton().GetMaterial(materialId);

	RAYLIB::Shader shader = DataPacks::singleton().GetShader(material->shaderId->getInstance());

	Engine::Components::Vector3 eulerAngles = this->transform->rotation.ToEulerRadians();

	model.transform = RAYMATH::MatrixRotateXYZ({
		eulerAngles.x,
		eulerAngles.y,
		eulerAngles.z
	});

	if (material->GetBaseColor() != nullptr && material->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc)
		this->tint = ((Engine::Components::Locs::ColorLoc^)material->GetBaseColor())->color;

	material->ApplyToShader(shader);

	for (int x = 0; x < model.materialCount; x++)
	{
		model.materials[x].shader = shader;
	}

	DrawModelShaderEx(
		model,
		{ transform->position.x,transform->position.y, transform->position.z },
		{ 0,0,0 },
		0.0f,
		transform->scale.toNative(),
		tint->toNative()
	);

	material->ResetShader(shader);
}

void ModelRenderer::Destroy()
{
	tint = nullptr;
	delete modelPtr;
}

RAYLIB::Model& Engine::EngineObjects::Geometry::ModelRenderer::GetModel()
{
	return this->modelPtr->getInstance();
}

RAYLIB::Model* Engine::EngineObjects::Geometry::ModelRenderer::GetModelPtr()
{
	return this->modelPtr->getPointer();
}

void Engine::EngineObjects::Geometry::ModelRenderer::OnModelIdChanged(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	RAYLIB::Model model = DataPacks::singleton().GetModel(newId);
	modelPtr->setInstance(CopyModel(model));
}