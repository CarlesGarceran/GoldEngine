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

void ModelRenderer::Setup()
{
	Engine::EngineObjects::Geometry::Abstract::Renderer::Setup();

	RAYLIB::Model& model = DataPacks::singleton().GetModel(modelId);

	this->model = new EnginePtr<RAYLIB::Model>(model);
	this->tint = Engine::Components::Color::New();
	this->attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &ModelRenderer::onModelUpdated));
}

void ModelRenderer::Draw()
{
	RAYLIB::Model& meshOnlyModel = this->model->getInstance();
	Engine::Components::Material^ material = DataPacks::singleton().GetMaterial(materialId);

	RAYLIB::Shader shader = DataPacks::singleton().GetShader(material->shaderId->getInstance());

	meshOnlyModel.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->rotation.x,
		DEG2RAD * this->transform->rotation.y,
		DEG2RAD * this->transform->rotation.z
	});

	if (material->GetBaseColor() != nullptr && material->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc)
		this->tint = ((Engine::Components::Locs::ColorLoc^)material->GetBaseColor())->color;

	material->ApplyToShader(shader);

	for (int x = 0; x < meshOnlyModel.materialCount; x++)
	{
		meshOnlyModel.materials[x].shader = shader;
		for (int y = 0; y < 12; y++)
		{
			meshOnlyModel.materials[x].maps[y] = MaterialMap();
		}
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