/*
#include "GeometryBuffer.h"
#include "../Objects/ModelRenderer/ModelRenderer.hpp"
#include "../Objects/MeshRenderer/MeshRenderer.h"

struct ModelDrawCall
{
	RAYLIB::Model model;
	msclr::gcroot<Engine::EngineObjects::Geometry::Abstract::Renderer^> renderer;
};

std::unordered_map<msclr::gcroot<Engine::Components::Material^>, std::vector<ModelDrawCall>> modelDrawcalls;

struct MaterialQueueCompare
{
	bool operator()(
		const decltype(modelDrawcalls.begin())& a,
		const decltype(modelDrawcalls.begin())& b) const
	{
		return a->first->renderQueue < b->first->renderQueue;
	}
};

static void Add(Engine::Components::Material^ material, ModelDrawCall mdl)
{
	if (modelDrawcalls.count(material) <= 0) modelDrawcalls[material] = std::vector<ModelDrawCall>();

	modelDrawcalls[material].push_back(mdl);
}


Engine::Render::GeometryBuffer::GeometryBuffer()
{
	Singleton<GeometryBuffer^>::Create(this);
	modelDrawcalls = std::unordered_map<msclr::gcroot<Engine::Components::Material^>, std::vector<ModelDrawCall>>();
}

void Engine::Render::GeometryBuffer::ClearBuffer()
{
	modelDrawcalls.clear();
}

void Engine::Render::GeometryBuffer::SubmitToBuffer(Engine::EngineObjects::Geometry::Abstract::Renderer^ renderer)
{
	RAYLIB::Model model = renderer->GetModel();
	Engine::Components::Material^ material = renderer->sharedMaterial;

	if (renderer->IsA<Engine::EngineObjects::Geometry::MeshRenderer^>())
	{
	}
	else
	{
		ModelDrawCall modelDrawCall{};
		modelDrawCall.model = model;
		modelDrawCall.renderer = renderer;

		Add(material, modelDrawCall);
	}
}

void Engine::Render::GeometryBuffer::Render(Engine::EngineObjects::Camera^ viewPoint)
{
	std::vector<decltype(modelDrawcalls.begin())> items;
	items.reserve(modelDrawcalls.size());

	for (auto it = modelDrawcalls.begin(); it != modelDrawcalls.end(); ++it)
		items.push_back(it);

	std::sort(items.begin(), items.end(), MaterialQueueCompare());


	for (auto& it : items)
	{
		Engine::Components::Material^ material = it->first;
		auto& models = it->second;

		bool useInstancing = material->GPUInstancing;

		RAYLIB::Shader shader = DataPacks::singleton().GetShader(material->shaderId->getInstance());

		material->ApplyToShader(shader);
		if (!useInstancing) 
		{
			for (auto& _model : models)
			{
				auto& renderer = _model.renderer;
				auto& model = _model.model;

				Engine::Components::Vector3 eulerAngles = renderer->transform->rotation.ToEulerRadians();

				model.transform = RAYMATH::MatrixRotateXYZ({
					eulerAngles.x,
					eulerAngles.y,
					eulerAngles.z
				});

				RAYLIB::DrawModelShader(
					model,
					renderer->transform->position.toNative(),
					1.0f,
					renderer->Tint->toNative()
				);
			}
		}
		material->ResetShader(shader);
	}
}

*/