#include "SDK.h"
#include "InstanceReference.h"

using namespace Engine::Internal::Components;
using namespace Engine::Scripting;


generic <class T> where T : Engine::Internal::Components::GameObject
InstanceReference<T>::InstanceReference(T instance)
{
	if (instance == nullptr) InstanceID = "";
	else this->InstanceID = instance->transform->GetUID();
}

generic <class T>
bool Engine::Scripting::InstanceReference<T>::HasInstance()
{
	return Instance != nullptr;
}

generic <class T> where T : Engine::Internal::Components::GameObject
T InstanceReference<T>::Instance::get()
{
	if (InstanceID == "") return T();

	GameObject^ instance = Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(InstanceID);

	if (instance == nullptr) return T();

	return instance->As<T>();
}


generic <class T> where T : Engine::Internal::Components::GameObject
void InstanceReference<T>::Instance::set(T value)
{
	if (value == nullptr) this->InstanceID = "";
	else this->InstanceID = value->transform->GetUID();
}

generic <class T>
T Engine::Scripting::InstanceReference<T>::operator->()
{
	return Instance;
}

generic <class T>
Engine::Scripting::InstanceReference<T>::operator T(InstanceReference<T> value)
{
	return value.Instance;
}

generic <class T>
Engine::Scripting::InstanceReference<T>::operator InstanceReference<T>(T value)
{
	return InstanceReference<T>(value);
}

