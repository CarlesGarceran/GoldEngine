#include "ReflectedType.h"
#include "ReflectableType.h"

using namespace Engine::Reflectable::Generic;

generic <class T>
Reflectable<T>::Reflectable(T inst)
{
	Instance = inst;
	type = gcnew Engine::Reflectable::ReflectableType(Instance->GetType());
	serializedObject = Newtonsoft::Json::JsonConvert::SerializeObject(Instance);
}

generic <class T>
T Engine::Reflectable::Generic::Reflectable<T>::getInstance()
{
	return this->Instance;
}

generic <class T>
void Engine::Reflectable::Generic::Reflectable<T>::setInstance(T instance)
{
	this->Instance = instance;
}

generic <class T>
void Reflectable<T>::deserialize()
{
	type->DeserializeType();
	Instance = (T)Newtonsoft::Json::JsonConvert::DeserializeObject(serializedObject, type->getTypeReference());
}

generic <class T>
void Engine::Reflectable::Generic::Reflectable<T>::serialize()
{
	serializedObject = Newtonsoft::Json::JsonConvert::SerializeObject(Instance);
}

generic <class T>
T Engine::Reflectable::Generic::Reflectable<T>::operator()()
{
	return Instance;
}

generic <class T>
void Reflectable<T>::operator=(T value)
{
	Instance = value;
}