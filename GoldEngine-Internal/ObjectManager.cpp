#include "SDK.h"
#include "ObjectManager.h"
#include <functional>

using namespace Engine::Scripting;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace Concurrency;

#pragma managed(push, off)

#pragma managed(pop)

void Engine::Scripting::ObjectManager::SwapScene(Engine::Management::Scene^ scene)
{
	this->loadedScene = scene;
}

ObjectManager::ObjectManager(Engine::Management::Scene^ loadedScene)
{
	this->loadedScene = loadedScene;
	Singleton<ObjectManager^>::Create(this);

	self = this;
}

bool waitDatamodel(std::string str)
{
	while (ObjectManager::singleton()->GetDatamodel(gcnew String(str.c_str())))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return true;
}

#pragma region HELPER_CLASSES

private ref class WaitForDatamodelTaskHelper
{
private:
	String^ datamodelName;

public:
	WaitForDatamodelTaskHelper(String^ datamodelName)
	{
		this->datamodelName = datamodelName;
	}

public:
	bool WaitForDatamodelWrapper()
	{
		return waitDatamodel(CastStringToNative(datamodelName));
	}
};

#pragma endregion

concurrency::task<bool> ObjectManager::WaitForDatamodel(String^ datamodelName)
{
	return task<bool>(
		std::bind(&waitDatamodel, CastStringToNative(datamodelName)
		)
	);
}

System::Threading::Tasks::Task<bool>^ ObjectManager::WaitForDatamodelAsync(String^ datamodelName)
{
	return System::Threading::Tasks::Task<bool>::Factory->StartNew(
		gcnew System::Func<bool>(gcnew WaitForDatamodelTaskHelper(datamodelName), &WaitForDatamodelTaskHelper::WaitForDatamodelWrapper));
}

List<GameObject^>^ ObjectManager::GetObjectsFromDatamodel(String^ datamodel)
{
	auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (topReferenceIsDatamodel(t, datamodel))
		{
			objects->Add(t);
		}
	}

	return objects;
}

List<Engine::Internal::Components::GameObject^>^ Engine::Scripting::ObjectManager::GetObjectsByLayer(Engine::Components::Layer^ layer)
{
	auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (t->layerMask->IsLayer(layer))
		{
			objects->Add(t);
		}
	}

	return objects;
}

List<Engine::Internal::Components::GameObject^>^ Engine::Scripting::ObjectManager::GetObjectsByTag(System::String^ tag)
{
	auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (t->GetTag() == tag)
		{
			objects->Add(t);
		}
	}

	return objects;
}

List<Engine::Internal::Components::GameObject^>^ Engine::Scripting::ObjectManager::GetObjectsByName(System::String^ name)
{
	auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

	if (loadedScene->GetRenderQueue() == nullptr)
		return nullptr;

	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (t->name == name)
		{
			objects->Add(t);
		}
	}

	return objects;
}

List<Engine::Internal::Components::GameObject^>^ Engine::Scripting::ObjectManager::GetObjects()
{
	auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (t != nullptr)
			objects->Add(t);
	}

	return objects;
}

GameObject^ ObjectManager::GetDatamodel(String^ dataModelName)
{
	return GetDatamodel(dataModelName, false);
}

GameObject^ ObjectManager::GetDatamodel(String^ dataModelName, bool createDataModel)
{
	return loadedScene->GetDatamodelMember(dataModelName, createDataModel);
}

void GetDescendantsOfRef(Engine::Internal::Components::GameObject^ parent, List<GameObject^>^% instances)
{
	for each (GameObject ^ instance in parent->GetChildren())
	{
		instances->Add(instance);
		GetDescendantsOfRef(instance, instances);
	}
}

List<Engine::Internal::Components::GameObject^>^ Engine::Scripting::ObjectManager::GetDescendantsOf(Engine::Internal::Components::GameObject^ parent)
{
	System::Collections::Generic::List<GameObject^>^ instances = gcnew System::Collections::Generic::List<GameObject^>();

	for each (GameObject^ instance in parent->GetChildren())
	{
		instances->Add(instance);
		GetDescendantsOfRef(instance, instances);
	}

	return instances;
}

Engine::EngineObjects::Camera^ ObjectManager::GetMainCamera()
{
	return GetMainCamera(false);
}

Engine::EngineObjects::Camera^ ObjectManager::GetMainCamera(bool ignoreEditorCameras)
{
	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		if (t->GetType()->IsSubclassOf(Engine::EngineObjects::Camera::typeid))
		{
#if !defined(PRODUCTION_BUILD)
			if (ignoreEditorCameras == true)
			{
				if (t->GetType() == Engine::EngineObjects::Editor::EditorCamera::typeid)
					continue;
			}
#endif

			if (((Engine::EngineObjects::Camera^)t)->IsMainCamera)
			{
				return (Engine::EngineObjects::Camera^)t;
			}
		}
	}

	return nullptr;
}

void Engine::Scripting::ObjectManager::Instantiate(Engine::Internal::Components::GameObject^ newObject)
{
	loadedScene->AddObjectToScene(newObject);
}

void ObjectManager::Destroy(Engine::Internal::Components::GameObject^ object)
{
	for each (GameObject ^ t in loadedScene->GetRenderQueue())
	{
		auto v = t;

		if (v != nullptr)
		{
			if (v == object)
			{
				auto type = v->GetObjectType();

				if (Engine::Management::Scene::getLoadedScene() != nullptr) // If the scene singleton is null, the scene is being unloaded
				{
					if (type == Engine::Internal::Components::ObjectType::Datamodel || type == Engine::Internal::Components::ObjectType::Daemon || type == Engine::Internal::Components::ObjectType::LightManager || v->isProtected())
						return;
				}


				// REPARENT ALL THE CHILDREN TO NULL (SET AS UNPARENTED).
				List<Engine::Internal::Components::GameObject^>^ objectList = ObjectManager::singleton()->GetChildrenOf(object);

				for each (auto obj in objectList)
				{
					obj->getTransform()->SetParent(nullptr);
				}

				// call destroy method (for self impl)
				object->Destroy();
				object->SetParent(nullptr);

				// PURGE THE OBJECT FROM THE SCENE
				loadedScene->derreferenceObject(loadedScene->getSceneObject(t));

				System::GC::Collect();
				System::GC::WaitForPendingFinalizers();
				break;
			}
		}
	}
}