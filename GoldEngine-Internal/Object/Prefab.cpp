#include "../SDK.h"
#include "Prefab.h"

using namespace Engine::Management;

void fixParentInstance(List<GameObject^>^% root, String^ oldId, String^ newTransform)
{
	for each (GameObject^ obj in root)
	{
		if (obj->transform->getParent() != nullptr)
		{
			if (obj->transform->GetParent()->GetUID() == oldId)
				obj->transform->GetParent()->SetUID(newTransform);
		}
	}
}

List<GameObject^>^ fixInstance(List<GameObject^>^% instances)
{
	List<GameObject^>^ fixedInstances = gcnew List<GameObject^>();

	for each (GameObject^ object in instances)
	{
		String^ uid = object->transform->GetUID();
		String^ newUid = System::Guid::NewGuid().ToString();
		object->transform->SetUID(newUid);
		fixParentInstance(instances, uid, newUid);

		fixedInstances->Add(object);
	}

	return fixedInstances;
}

List<GameObject^>^ fetchEveryChild(GameObject^ instance, List<GameObject^>^ list = nullptr)
{
	List<GameObject^>^ instances;

	if (list == nullptr)
		instances = gcnew List<GameObject^>();
	else
		instances = list;

	for each (GameObject ^ child in instance->childs)
	{
		instances->Add(child);
		fetchEveryChild(child, instances);
	}

	return instances;
}

Prefab::Prefab(GameObject^ instance)
{
	this->gameInstance->Add(instance);

	// serialize

	List<GameObject^>^ childInstances = fetchEveryChild(instance, nullptr);

	for each (GameObject^ obj in childInstances)
	{
		this->gameInstance->Add(obj);
	}

	for each (GameObject^ object in this->gameInstance)
	{
		this->instance->Add(Serialize(object), object->InstanceType);
	}
}

[Newtonsoft::Json::JsonConstructorAttribute]
Prefab::Prefab()
{

}

GameObject^ Prefab::getInstance() 
{
	return this->gameInstance[0]; // the root object
}

List<GameObject^>^ Prefab::getInstances()
{
	return this->gameInstance; // the entire collection of objects
}


Type^ Prefab::getObjectType() 
{ 
	return nullptr;
}

void Prefab::DeserializeObject()
{
	for each (String^ key in this->instance->Keys)
	{
		Engine::Reflectable::ReflectableType^ type = nullptr;

		if (this->instance->TryGetValue(key, type))
		{
			this->gameInstance->Add((GameObject^)Deserialize(key, type->getTypeReference()));
		}
	}


	this->gameInstance = fixInstance(gameInstance);
}

List<GameObject^>^ Prefab::LoadPrefab(String^ prefabPath) 
{
	Prefab^ p = Deserialize<Prefab^>(File::ReadAllText(prefabPath));
	p->DeserializeObject();

	List<GameObject^>^ instances = p->getInstances();
	instances[0]->transform->setParent(Singleton<Engine::Scripting::ObjectManager^>::Instance->GetDatamodel("workspace", false)->transform);

	fixInstance(instances);

	return instances;
}

Prefab^ Prefab::GetPrefab(String^ prefabPath)
{
	Prefab^ p = Deserialize<Prefab^>(File::ReadAllText(prefabPath));
	p->DeserializeObject();

	return p;
}