#include "../../SDK.h"
#include "../Abstract/Renderer.h"
#include "ModelRenderer.hpp"

using namespace Engine::EngineObjects::Geometry;
using namespace Engine::Assets::Storage;
using namespace Engine::Native;

Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer()
	: Engine::EngineObjects::Geometry::Abstract::Renderer()
{

}

ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform)
	: ModelRenderer::ModelRenderer(name, transform,0,0, Engine::Components::Color::New())
{
	
}

Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId)
	: ModelRenderer::ModelRenderer(name, transform, modelId, materialId, Engine::Components::Color::New())
{
	
}

Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId, Engine::Components::Color^ tint)
	: Engine::EngineObjects::Geometry::Abstract::Renderer(name, transform)
{
	this->modelId = modelId;
	this->materialId = materialId;
	this->tint = tint;
}

void ModelRenderer::Setup()
{
	RAYLIB::Model& model = DataPacks::singleton().GetModel(modelId);

	this->model = new EnginePtr<RAYLIB::Model>(model);
	this->material = DataPacks::singleton().GetMaterial(materialId);

	if (this->tint == nullptr)
		(this->material->GetBaseColor() != nullptr && this->material->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) ? this->tint = ((Engine::Components::Locs::ColorLoc^)this->material->GetBaseColor())->color : this->tint = Engine::Components::Color::New();

	this->attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &ModelRenderer::onModelUpdated));
	this->attributes->getAttribute("materialId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &ModelRenderer::onMaterialUpdated));
}

void ModelRenderer::Draw()
{
	RAYLIB::Model meshOnlyModel = this->model->getInstance();
	RAYLIB::Shader shader = DataPacks::singleton().GetShader(this->material->shaderId->getInstance());

	meshOnlyModel.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation.x,
		DEG2RAD * this->transform->rotation.y,
		DEG2RAD * this->transform->rotation.z
	});

	if (this->material->GetBaseColor() != nullptr && this->material->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc)
		this->tint = ((Engine::Components::Locs::ColorLoc^)this->material->GetBaseColor())->color;

	material->ApplyToShader(shader);

	for (int x = 0; x < meshOnlyModel.materialCount; x++)
	{
		meshOnlyModel.materials[x].shader = shader;
	}

	DrawModelEx(
		meshOnlyModel,
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
	delete model;
	tint = nullptr;
	material = nullptr;
}

RAYLIB::Model* Engine::EngineObjects::Geometry::ModelRenderer::GetModel()
{
	return &model->getInstance();
}

void Engine::EngineObjects::Geometry::ModelRenderer::onModelUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	RAYLIB::Model& model = DataPacks::singleton().GetModel(newId);

	if (this->model == nullptr)
	{
		this->model = new EnginePtr<RAYLIB::Model>(model);
		return;
	}

	for (int x = 0; x < model.materialCount; x++)
	{
		RAYLIB::Material defaultMaterial = RAYLIB::LoadMaterialDefault();
		model.materials[x] = defaultMaterial;
	}

	this->model->setInstance(model);
}

void Engine::EngineObjects::Geometry::ModelRenderer::onMaterialUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	this->material = DataPacks::singleton().GetMaterial(newId);

	if (this->tint == nullptr)
		(this->material->GetBaseColor() != nullptr && this->material->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) ? this->tint = ((Engine::Components::Locs::ColorLoc^)this->material->GetBaseColor())->color : this->tint = Engine::Components::Color::New();
}