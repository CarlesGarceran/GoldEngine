#include "ReflectedType.h"
#include "ReflectableType.h"

using namespace Engine::Reflectable::Generic;

generic <class T>
Reflectable<T>::Reflectable(T inst)
{
	Instance = inst;
	type = gcnew Engine::Reflectable::ReflectableType(Instance->GetType());
}

generic <class T>
void Reflectable<T>::deserialize()
{
	type->DeserializeType();
	Instance = (T)System::Convert::ChangeType(Instance, type->getTypeReference());
}

generic <class T>
T Reflectable<T>::operator->()
{
	return Instance;
}

generic <class T>
T Reflectable<T>::operator=(T value)
{
	Instance = value;
	return Instance;
}

generic <class T>
void Reflectable<T>::operator[](T value)
{
	Instance = value;
}