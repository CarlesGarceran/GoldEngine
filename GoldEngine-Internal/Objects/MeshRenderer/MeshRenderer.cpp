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

void MeshRenderer::Setup()
{
	Engine::EngineObjects::Geometry::Abstract::Renderer::Setup();

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
	
	this->attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &MeshRenderer::onModelUpdated));
	this->attributes->getAttribute("meshIndex")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &MeshRenderer::onMeshIndexUpdated));
	this->attributes->getAttribute("materialId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &MeshRenderer::onMaterialUpdated));
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
	RAYLIB::Model meshOnlyModel = this->modelInstance->getInstance();
	RAYLIB::Shader shader = DataPacks::singleton().GetShader(this->materialInstance->shaderId->getInstance());

	meshOnlyModel.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation.x,
		DEG2RAD * this->transform->rotation.y,
		DEG2RAD * this->transform->rotation.z
	});

	if (this->materialInstance->GetBaseColor() != nullptr && this->materialInstance->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) 
		this->tint = ((Engine::Components::Locs::ColorLoc^)this->materialInstance->GetBaseColor())->color;

	materialInstance->ApplyToShader(shader);

	meshOnlyModel.materials[0].shader = shader;

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

	materialInstance->ResetShader(shader);
}

void MeshRenderer::Destroy()
{
	delete modelInstance;
	delete meshInstance;
	tint = nullptr;
	materialInstance = nullptr;
}

RAYLIB::Model* Engine::EngineObjects::Geometry::MeshRenderer::GetModel()
{
	return &modelInstance->getInstance();
}

void Engine::EngineObjects::Geometry::MeshRenderer::onModelUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	RAYLIB::Model& model = DataPacks::singleton().GetModel(newId);

	if (this->modelInstance == nullptr)
	{
		this->modelInstance = new Engine::Native::EnginePtr<RAYLIB::Model>(model);
		return;
	}

	for (int x = 0; x < model.materialCount; x++)
	{
		RAYLIB::Material defaultMaterial = RAYLIB::LoadMaterialDefault();
		model.materials[x] = defaultMaterial;
	}

	this->modelInstance->setInstance(model);
}

void Engine::EngineObjects::Geometry::MeshRenderer::onMaterialUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	this->materialInstance = DataPacks::singleton().GetMaterial(newId);

	if (this->tint == nullptr)
		(this->materialInstance->GetBaseColor() != nullptr && this->materialInstance->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) ? this->tint = ((Engine::Components::Locs::ColorLoc^)this->materialInstance->GetBaseColor())->color : this->tint = Engine::Components::Color::New();
}

void Engine::EngineObjects::Geometry::MeshRenderer::onMeshIndexUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;
	RAYLIB::Model& model = DataPacks::singleton().GetModel(newId);

	if (this->meshInstance == nullptr)
	{
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(model.meshes[newId]);
		return;
	}

	this->meshInstance->setInstance(model.meshes[newId]);
}
