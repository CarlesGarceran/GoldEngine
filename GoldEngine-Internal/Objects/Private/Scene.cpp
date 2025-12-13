#include "../../SDK.h"
#include "Scene.h"

using namespace Engine::EngineObjects::Private;

Scene::Scene() : 
	Engine::EngineObjects::Script(),
	scenePtr(Engine::Management::Scene::getLoadedScene()),
	sceneName(scenePtr->sceneName),
	skyColor(gcnew Engine::Components::Color(scenePtr->skyColor)),
	layerMasks(gcnew System::Collections::Generic::List<Engine::Components::Layer^>())
{
	protectMember();
	this->name = "game";
}

void Engine::EngineObjects::Private::Scene::Awake()
{
	if(layerMasks == nullptr)
	{
		layerMasks = Engine::Scripting::LayerManager::GetLayers();
	}
	else
	{
		if (layerMasks->Count > 0) 
		{
			Engine::Scripting::LayerManager::LoadLayers(layerMasks);
		}
		else
		{
			layerMasks = Engine::Scripting::LayerManager::GetLayers();
		}
	}
}

GameObject^ Engine::EngineObjects::Private::Scene::GetService(System::String^ serviceName)
{
	return GetChild(serviceName);
}

void Scene::Update()
{
	if (!isProtected())
		protectMember();

	Engine::Management::Scene::getLoadedScene()->sceneName = sceneName;
	Engine::Management::Scene::getLoadedScene()->skyColor = this->skyColor->toHex();
	layerMasks = Engine::Scripting::LayerManager::GetLayers();
}