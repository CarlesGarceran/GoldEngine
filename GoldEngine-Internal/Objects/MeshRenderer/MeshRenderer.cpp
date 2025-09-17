#include "../../SDK.h"
#include "../Abstract/Renderer.h"
#include "MeshRenderer.h"

using namespace Engine::Scripting;
using namespace Engine::Assets::Storage;

using namespace Engine::EngineObjects::Geometry;

MeshRenderer::MeshRenderer(String^ name, Engine::Internal::Components::Transform^ transform)
	: Engine::EngineObjects::Geometry::Abstract::Renderer(name, transform)
{

}

MeshRenderer::MeshRenderer()
	: Engine::EngineObjects::Geometry::Abstract::Renderer()
{

}

void MeshRenderer::Start()
{
	// RESOURCE ALLOCATION

	if (this->tint == nullptr)
		this->tint = gcnew Engine::Components::Color();

	RAYLIB::Model& modelCopy = DataPacks::singleton().GetModel(modelId);

	this->materialInstance = DataPacks::singleton().GetMaterial(materialId);

	if (modelCopy.meshCount <= meshIndex)
		meshIndex = modelCopy.meshCount;
	else if (meshIndex < 0)
		meshIndex = 0;

	this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(modelCopy.meshes[meshIndex]);
	this->modelInstance = new Engine::Native::EnginePtr<RAYLIB::Model>(RAYLIB::LoadModelFromMesh(this->meshInstance->getInstance()));
}

[Engine::Attributes::ExecuteInEditModeAttribute]
void MeshRenderer::Update()
{
	RAYLIB::Model& modelCopy = DataPacks::singleton().GetModel(modelId);

	if (modelCopy.meshCount <= meshIndex)
		meshIndex = modelCopy.meshCount - 1;
	else if (meshIndex < 0)
		meshIndex = 0;

	if (this->meshInstance == nullptr)
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(modelCopy.meshes[meshIndex]);
	else
		this->meshInstance->setInstance(modelCopy.meshes[meshIndex]);

	if (this->modelInstance == nullptr)
		this->modelInstance = new Engine::Native::EnginePtr<RAYLIB::Model>(RAYLIB::LoadModelFromMesh(this->meshInstance->getInstance()));
	else
		this->modelInstance->setInstance(RAYLIB::LoadModelFromMesh(this->meshInstance->getInstance()));

	this->materialInstance = DataPacks::singleton().GetMaterial(materialId);
}

void MeshRenderer::Draw()
{
	RAYLIB::Model& meshOnlyModel = this->modelInstance->getInstance();
	RAYLIB::Shader& shader = DataPacks::singleton().GetShader(this->materialInstance->shaderId->getInstance());

	meshOnlyModel.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation.x,
		DEG2RAD * this->transform->rotation.y,
		DEG2RAD * this->transform->rotation.z
	});

	if (this->materialInstance->GetBaseColor() != nullptr && this->materialInstance->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) 
		this->tint = ((Engine::Components::Locs::ColorLoc^)this->materialInstance->GetBaseColor())->color;

	meshOnlyModel.materials[0].shader = shader;

	materialInstance->ApplyToShader(shader);

	RAYMATH::Matrix translation = RAYMATH::MatrixTranslate(
		0,
		0,
		0
	);

	RAYMATH::Matrix rotation = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation.x,
		DEG2RAD * this->transform->rotation.y,
		DEG2RAD * this->transform->rotation.z
		});

	RAYLIB::Matrix scale = RAYMATH::MatrixScale(
		this->transform->scale.x,
		this->transform->scale.y,
		this->transform->scale.z
	);

	RAYLIB::Matrix _transform = RAYMATH::MatrixMultiply(
		scale, 
		RAYMATH::MatrixMultiply(
			rotation,
			translation
		)
	);

	meshOnlyModel.transform = _transform;

	DrawModelEx(
		meshOnlyModel,
		{ transform->position.x,transform->position.y, transform->position.z },
		{ 0,0,0 },
		0.0f,
		transform->scale.toNative(),
		tint->toNative()
	);
	
}

void MeshRenderer::Destroy()
{
	delete modelInstance;
	delete meshInstance;
	tint = nullptr;
	materialInstance = nullptr;
}

RAYLIB::Model& Engine::EngineObjects::Geometry::MeshRenderer::GetModel()
{
	return modelInstance->getInstance();
}
