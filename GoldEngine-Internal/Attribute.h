#pragma once

#include "Event.h"
#include "LoggingAPI.h"

namespace Engine::Scripting
{
	public enum class AccessLevel
	{
		Public,
		ReadOnly,
		WriteOnly
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
	public ref class Attribute
	{
	public:
		AccessLevel accessLevel;
		String^ name;
		Engine::Reflectable::ReflectableType^ userDataType;
		[Newtonsoft::Json::JsonIgnoreAttribute]
		Engine::Scripting::Events::Event^ onPropertyChanged;

	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		System::Object^ userData;

		System::Reflection::MemberInfo^ descriptor;
		System::Object^ rootObject;
		
	public:
		Attribute(String^ str, System::Object^ data);
		[Newtonsoft::Json::JsonConstructorAttribute]
		Attribute(AccessLevel level, String^ str, System::Object^ data, System::Type^ dT);

	public:
		void setValue(System::Object^ object, bool overrideType);
		void setValueForce(System::Object^ object, bool overrideType);

		void setValue(System::Object^ object) { setValue(object, false); }
		void setValueForce(System::Object^ object) { setValueForce(object, false); }

		void synchronizeDescriptor();
		void setPropertyDescriptor(System::Reflection::MemberInfo^ descriptor, System::Object^ rootDescriptor);

	public:
		System::Object^ getValue()
		{
			if (userData == nullptr)
				return nullptr;

			if (accessLevel == AccessLevel::WriteOnly)
				return nullptr;

			return userData;
		}

		generic <class T>
		T getValue()
		{
			if (userData == nullptr)
				return T();

			if (accessLevel == AccessLevel::WriteOnly)
				return { };

			return (T)userData;
		}

		generic <class T>
		T getValueAs()
		{
			if (accessLevel == AccessLevel::WriteOnly)
				return { };

			return (T)userData;
		}

		generic <class T>
		T getValueFromJObject()
		{
			if (accessLevel == AccessLevel::WriteOnly)
				return { };

			return (T)(getValueAs<Newtonsoft::Json::Linq::JObject^>()->ToObject<T>());
		}

		bool isJObject()
		{
			return userData->GetType() == Newtonsoft::Json::Linq::JObject::typeid;
		}

		auto getValueAuto()
		{
			if (accessLevel == AccessLevel::WriteOnly)
				return gcnew System::Object();

			if(userDataType->getTypeReference() != nullptr)
				return Convert::ChangeType(userData, userDataType->getTypeReference());
			else
				return Convert::ChangeType(userData, System::Object::typeid);
		}

		System::Type^ getValueType()
		{
			return this->userDataType->getTypeReference();
		}

		System::Type^ getCurrentType()
		{
			return this->userData->GetType();
		}

		void lockRead();
		void lockWrite();
		void unlock();

	public:
		System::Object^ DeserializeAttribute()
		{
			if (userData == nullptr)
				return userData;

			if(userDataType != nullptr)
				userDataType->DeserializeType();

			if (userData->GetType() != Newtonsoft::Json::Linq::JObject::typeid)
				return userData;

			try
			{
				Newtonsoft::Json::Linq::JObject^ sonObject = (Newtonsoft::Json::Linq::JObject^)userData;
				return sonObject->ToObject(userDataType->getTypeReference());
			}
			catch (Exception^ ex)
			{
			#ifdef LOGAPI_IMPL
				printError(ex->Message);
				printError(ex->StackTrace);
			#endif

				return userData;
			}
		}

		void setType(System::Type^ type)
		{
			if (userData == nullptr)
				return;

			if (userData->GetType() == type)
				return;

			try 
			{
			#ifdef LOGAPI_IMPL
				printConsole("Converting from " + userData->GetType()->Name + " To -> " + type->Name);
			#endif
				userDataType->SetType(type);

				userData = System::Convert::ChangeType(userData, type);
			}
			catch (Exception^ ex)
			{
			#ifdef LOGAPI_IMPL

			#endif
			}
		}

		void resetType()
		{
			try
			{
				if (userData == nullptr)
					return;

#ifdef LOGAPI_IMPL
				printConsole("Converting from " + userData->GetType()->Name + " To -> " + userDataType->getTypeReference()->Name);
#endif
				userData = System::Convert::ChangeType(userData, userDataType->getTypeReference());
			}
			catch (Exception^ ex)
			{
#ifdef LOGAPI_IMPL

#endif
			}
		}


	public:
		static Attribute^ create(String^ name, System::Object^ data, Type^ type)
		{
			return New(AccessLevel::Public, name, data, type);
		}

		static Attribute^ create(AccessLevel accessLevel, String^ name, System::Object^ data, Type^ type)
		{
			return New(accessLevel, name, data, type);
		}

		static Attribute^ create(String^ name, System::Object^ data)
		{
			return New(AccessLevel::Public, name, data);
		}

		static Attribute^ create(AccessLevel accessLevel, String^ name, System::Object^ data)
		{
			return New(accessLevel, name, data);
		}

		static Attribute^ New(AccessLevel level, String^ name, System::Object^ data)
		{
			return gcnew Attribute(level, name, data, data->GetType());
		}

		static Attribute^ New(AccessLevel level, String^ name, System::Object^ data, Type^ type)
		{
			return gcnew Attribute(level, name, data, type);
		}
	};
}