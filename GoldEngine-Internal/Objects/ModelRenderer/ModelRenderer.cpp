#include "../../SDK.h"
#include "ModelRenderer.hpp"

using namespace Engine::EngineObjects::Geometry;
using namespace Engine::Assets::Storage;
using namespace Engine::Native;

ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform) 
	: ModelRenderer::ModelRenderer(name, transform,0,0, Engine::Components::Color::New())
{
	
}

Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId)
	: ModelRenderer::ModelRenderer(name, transform, modelId, materialId, Engine::Components::Color::New())
{
	
}

Engine::EngineObjects::Geometry::ModelRenderer::ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId, Engine::Components::Color^ tint)
	: Engine::EngineObjects::Script(name, transform)
{
	this->modelId = modelId;
	this->materialId = materialId;
	this->tint = tint;
}

void ModelRenderer::Start()
{
	if (this->tint == nullptr)
		this->tint = Engine::Components::Color::New();
	
	RAYLIB::Model& model = DataPacks::singleton().GetModel(modelId);

	this->model = new EnginePtr<RAYLIB::Model>(model);
	this->material = DataPacks::singleton().GetMaterial(materialId);

	this->attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &ModelRenderer::onModelUpdated));
	this->attributes->getAttribute("materialId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &ModelRenderer::onMaterialUpdated));
}

void ModelRenderer::Draw()
{
	RAYLIB::Model meshOnlyModel = this->model->getInstance();
	RAYLIB::Shader& shader = DataPacks::singleton().GetShader(this->material->shaderId->getInstance());

	meshOnlyModel.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation->x,
		DEG2RAD * this->transform->rotation->y,
		DEG2RAD * this->transform->rotation->z
		});

	meshOnlyModel.materials[0].shader = shader;

	material->ApplyToShader(shader);

	DrawModelEx(
		meshOnlyModel,
		{ transform->position->x,transform->position->y, transform->position->z },
		{ 0,0,0 },
		0.0f,
		transform->scale->toNative(),
		tint->toNative()
	);
}

void ModelRenderer::Destroy()
{
	delete model;
	tint = nullptr;
	material = nullptr;
}

void Engine::EngineObjects::Geometry::ModelRenderer::onModelUpdated(unsigned int newId, unsigned int oldId)
{
	RAYLIB::Model& model = DataPacks::singleton().GetModel(newId);

	if (this->model == nullptr)
	{
		this->model = new EnginePtr<RAYLIB::Model>(model);
		return;
	}

	this->model->setInstance(model);
}

void Engine::EngineObjects::Geometry::ModelRenderer::onMaterialUpdated(unsigned int newId, unsigned int oldId)
{
	this->material = DataPacks::singleton().GetMaterial(materialId);
}