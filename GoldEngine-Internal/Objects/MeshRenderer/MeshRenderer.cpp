#include "../../SDK.h"
#include "../Abstract/Renderer.h"
#include "MeshRenderer.h"

using namespace Engine::Scripting;
using namespace Engine::Assets::Storage;

using namespace Engine::EngineObjects::Geometry;

MeshRenderer::MeshRenderer()
	: Engine::EngineObjects::Geometry::Abstract::Renderer()
{

}

void MeshRenderer::Awake()
{
	if (this->tint == nullptr)
		this->tint = Engine::Components::Color::New();

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

	if (modelCopy.meshCount <= meshIndex)
		meshIndex = modelCopy.meshCount - 1;
	else if (meshIndex < 0)
		meshIndex = 0;

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
		this->tint = ((Engine::Components::Locs::ColorLoc^)materialInstance->GetBaseColor())->color;

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
	tint = nullptr;
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

	if (this->meshInstance == nullptr)
	{
		this->meshInstance = new Engine::Native::EnginePtr<RAYLIB::Mesh>(model.meshes[newId]);
		return;
	}

	this->meshInstance->setInstance(model.meshes[newId]);
}
