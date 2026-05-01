#include "Includes.h"
#include "Reflection/ReflectedType.h"
#include "GlIncludes.h"
#include "CastToNative.h"
#include "LoggingAPI.h"
#include "Instantiable.h"
#include "Event.h"
#include "Attribute.h"

using namespace System;

inline System::Type^ GetTypeFromDescriptor(System::Reflection::MemberInfo^ memberInfo, System::Type^ defaultType)
{
	System::Type^ targetType = defaultType;

	if (memberInfo == System::Reflection::FieldInfo::typeid)
		targetType = ((System::Reflection::FieldInfo^)memberInfo)->FieldType;

	if (memberInfo == System::Reflection::PropertyInfo::typeid)
		targetType = ((System::Reflection::PropertyInfo^)memberInfo)->PropertyType;

	return targetType;
}

inline Engine::Scripting::Attribute::Attribute(String^ str, System::Object^ data)
{
	/*
	if (data == nullptr)
	{
		delete this; // clean memory, fuck off
		return;
	}
	*/
	this->accessLevel = AccessLevel::Public;
	this->name = str;
	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();

	if (data != nullptr)
	{
		if (userDataType == nullptr)
			userDataType = gcnew Engine::Reflectable::ReflectableType(data->GetType());
		else
			userDataType->DeserializeType();
	}
	else
	{
		userDataType = gcnew Engine::Reflectable::ReflectableType();
	}

	if (data == Newtonsoft::Json::Linq::JObject::typeid)
	{
		userData = ((Newtonsoft::Json::Linq::JObject^)data)->ToObject<System::Object^>();
	}
	else
	{
		userData = data;
	}

	setType(userDataType->getTypeReference()); // For consistency
}

[Newtonsoft::Json::JsonConstructorAttribute]
inline Engine::Scripting::Attribute::Attribute(AccessLevel level, String^ str, System::Object^ data, System::Type^ dT)
{
	/*
	if (data == nullptr)
	{
		delete this; // clean memory, fuck off
		return;
	}
	*/
	this->accessLevel = level;
	this->name = str;
	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();

	if (data != nullptr)
	{
		if (userDataType == nullptr)
			userDataType = gcnew Engine::Reflectable::ReflectableType(data->GetType());
		else
			userDataType->DeserializeType();
	}
	else
	{
		userDataType = gcnew Engine::Reflectable::ReflectableType(dT);
	}

	if (data == Newtonsoft::Json::Linq::JObject::typeid)
	{
		userData = ((Newtonsoft::Json::Linq::JObject^)data)->ToObject<Object^>();
	}
	else
	{
		userData = data;
	}

	setType(userDataType->getTypeReference()); // For consistency
}

Engine::Scripting::Attribute::~Attribute()
{
	onPropertyChanged->disconnectAll();

	name = nullptr;
	userDataType = nullptr;
	onPropertyChanged = nullptr;
	descriptor = nullptr;
	rootObject = nullptr;
	userData = nullptr;
}

bool isEnum(System::Type^ type)
{
	return (type->IsEnum);
}

inline void Engine::Scripting::Attribute::synchronizeDescriptor()
{
	if (descriptor != nullptr && rootObject != nullptr)
	{
		System::Type^ descriptorType = GetTypeFromDescriptor(descriptor, userDataType->getTypeReference());
		
		if (descriptor->MemberType.Equals(System::Reflection::MemberTypes::Field))
		{
			if (!isEnum(descriptorType))
			{
				((System::Reflection::FieldInfo^)descriptor)->SetValue(rootObject, userData);
			}
			else
			{
				System::Object^ arg = System::Enum::Parse(descriptorType, userData->ToString(), false);

				((System::Reflection::FieldInfo^)descriptor)->SetValue(
					rootObject,
					arg
				);

				this->setValueForce(arg, true);
			}
		}
		else
		{
			if (!isEnum(descriptorType))
			{
				((System::Reflection::PropertyInfo^)descriptor)->SetValue(rootObject, userData);
			}
			else
			{
				System::Object^ arg = System::Enum::Parse(descriptorType, userData->ToString(), false);

				((System::Reflection::PropertyInfo^)descriptor)->SetValue(
					rootObject,
					arg
				);

				this->setValueForce(arg, true);
			}
		}
	}
}

inline void Engine::Scripting::Attribute::setPropertyDescriptor(System::Reflection::MemberInfo^ descriptor, System::Object^ rootDescriptor)
{
	this->descriptor = descriptor;
	this->rootObject = rootDescriptor;
}

inline void Engine::Scripting::Attribute::setValue(System::Object^ object, bool overrideType)
{
	if (accessLevel == AccessLevel::ReadOnly)
		return;

	if (object == userData || object->Equals(userData)) return;

	onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { object, userData });
	userData = object;
	
	if (descriptor != nullptr)
		if (descriptor->MemberType.Equals(System::Reflection::MemberTypes::Field))
			((System::Reflection::FieldInfo^)descriptor)->SetValue(rootObject, userData);
		else
			((System::Reflection::PropertyInfo^)descriptor)->SetValue(rootObject, userData);

	if (overrideType)
		userDataType->SetType(object->GetType());
}

inline void Engine::Scripting::Attribute::setValueForce(System::Object^ object, bool overrideType)
{
	if (object == userData || object->Equals(userData)) return;

	onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { object, userData });
	userData = object;

	if (descriptor != nullptr)
		if (descriptor->MemberType.Equals(System::Reflection::MemberTypes::Field))
			((System::Reflection::FieldInfo^)descriptor)->SetValue(rootObject, userData);
		else
			((System::Reflection::PropertyInfo^)descriptor)->SetValue(rootObject, userData);

	if (overrideType)
		userDataType->SetType(object->GetType());
}

inline void Engine::Scripting::Attribute::lockRead()
{
	accessLevel = AccessLevel::WriteOnly;
}

inline void Engine::Scripting::Attribute::lockWrite()
{
	accessLevel = AccessLevel::ReadOnly;
}

inline void Engine::Scripting::Attribute::unlock()
{
	accessLevel = AccessLevel::Public;
}