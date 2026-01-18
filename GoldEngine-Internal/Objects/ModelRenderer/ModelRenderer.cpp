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

void ModelRenderer::Awake()
{
	this->tint = Engine::Components::Color::New();
}

void ModelRenderer::Draw()
{
	RAYLIB::Model& meshOnlyModel = DataPacks::singleton().GetModel(modelId);
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
	}

	DrawModelShaderEx(
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
	tint = nullptr;
}

RAYLIB::Model* Engine::EngineObjects::Geometry::ModelRenderer::GetModel()
{
	return &DataPacks::singleton().GetModel(modelId);
}