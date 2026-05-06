#include "../../SDK.h"
#include "../Abstract/Renderer.h"
#include "MeshRenderer.h"

#include <math.h>

using namespace Engine::Scripting;
using namespace Engine::Assets::Storage;

using namespace Engine::EngineObjects::Geometry;

static unsigned int clamp(unsigned int t, unsigned int min, unsigned int max)
{
	if (t >= max) return max;
	if (t <= min) return min;

	return t;
}

MeshRenderer::MeshRenderer()
	: Engine::EngineObjects::Geometry::Abstract::Renderer()
{

}

void MeshRenderer::Awake()
{
	if (this->Tint == nullptr)
		this->Tint = Engine::Components::Color::New();

	RAYLIB::Model& modelCopy = DataPacks::singleton().GetModel(modelId);

	if (modelCopy.meshCount <= meshIndex)
		meshIndex = modelCopy.meshCount;
	else if (meshIndex < 0)
		meshIndex = 0;

	this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(modelCopy.meshes[meshIndex]);

	this->attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &MeshRenderer::onModelUpdated));
	this->attributes->getAttribute("meshIndex")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &MeshRenderer::onMeshIndexUpdated));
}

[Engine::Attributes::ExecuteInEditModeAttribute]
void MeshRenderer::Update()
{
	RAYLIB::Model& modelCopy = DataPacks::singleton().GetModel(modelId);

	meshIndex = clamp(meshIndex, 0, modelCopy.meshCount-1);
	this->attributes->getAttribute("meshIndex")->setValue(meshIndex, false);

	if (this->meshInstance == nullptr)
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(modelCopy.meshes[meshIndex]);
	else
		this->meshInstance->setInstance(modelCopy.meshes[meshIndex]);
}

void MeshRenderer::Draw()
{
	Engine::Components::Material^ materialInstance = DataPacks::singleton().GetMaterial(materialId);
	RAYLIB::Shader shader = DataPacks::singleton().GetShader(materialInstance->shaderId->getInstance());

	if (materialInstance->GetBaseColor() != nullptr && materialInstance->GetBaseColor()->GetLocType() == Engine::Components::Enums::MaterialLocations::ColorLoc) 
		this->Tint = ((Engine::Components::Locs::ColorLoc^)materialInstance->GetBaseColor())->color;

	materialInstance->ApplyToShader(shader);

	RAYMATH::Matrix translation = RAYMATH::MatrixTranslate(
		transform->position.x,
		transform->position.y,
		transform->position.z
	);

	Engine::Components::Vector3 eulerAngles = this->transform->rotation.ToEulerRadians();

	RAYMATH::Matrix rotation = RAYMATH::MatrixRotateXYZ({
		eulerAngles.x,
		eulerAngles.y,
		eulerAngles.z
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

	DrawMeshShader(
		meshInstance->getInstance(),
		shader,
		_transform
	); // From my Custom RLAPI

	materialInstance->ResetShader(shader);
}

void MeshRenderer::Destroy()
{
	delete meshInstance;
	meshInstance = nullptr;
	Tint = nullptr;
}

RAYLIB::Model& Engine::EngineObjects::Geometry::MeshRenderer::GetModel()
{
	return DataPacks::singleton().GetModel(modelId);
}

RAYLIB::Model* Engine::EngineObjects::Geometry::MeshRenderer::GetModelPtr()
{
	return &DataPacks::singleton().GetModel(modelId);
}

void Engine::EngineObjects::Geometry::MeshRenderer::onModelUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	RAYLIB::Model& model = DataPacks::singleton().GetModel(newId);

	if (this->meshInstance == nullptr)
	{
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(model.meshes[meshIndex]);
		return;
	}

	this->meshInstance->setInstance(model.meshes[meshIndex]);
}

void Engine::EngineObjects::Geometry::MeshRenderer::onMeshIndexUpdated(unsigned int newId, unsigned int oldId)
{
	if (newId == oldId) return;

	RAYLIB::Model& model = DataPacks::singleton().GetModel(modelId);

	newId = clamp(newId, 0, model.meshCount-1);

	if (this->meshInstance == nullptr)
	{
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(model.meshes[newId]);
		return;
	}

	this->meshInstance->setInstance(model.meshes[newId]);
}

Engine::Components::Material^ Engine::EngineObjects::Geometry::MeshRenderer::sharedMaterial::get()
{
	return DataPacks::singleton().GetMaterial(materialId);
}

cli::array<Engine::Components::Material^>^ Engine::EngineObjects::Geometry::MeshRenderer::sharedMaterials::get()
{
	return gcnew cli::array<Engine::Components::Material^>(1) { DataPacks::singleton().GetMaterial(materialId) };
}
