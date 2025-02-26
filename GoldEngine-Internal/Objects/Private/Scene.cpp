#include "../../SDK.h"
#include "Scene.h"

using namespace Engine::EngineObjects::Private;

Scene::Scene(String^ name, Engine::Internal::Components::Transform^ transform) : 
	Engine::EngineObjects::Script(name, transform),
	scenePtr(Engine::Management::Scene::getLoadedScene()),
	sceneName(scenePtr->sceneName),
	skyColor(gcnew Engine::Components::Color(scenePtr->skyColor))
{
	protectMember();
	this->name = "game";
}

void Scene::Update()
{
	if (!isProtected())
		protectMember();

	Engine::Management::Scene::getLoadedScene()->sceneName = sceneName;
	Engine::Management::Scene::getLoadedScene()->skyColor = this->skyColor->toHex();
}