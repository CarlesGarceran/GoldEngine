#pragma once

using namespace MoonSharp::Interpreter;

namespace Engine::Lua::VM
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute("VMWrap")]
		public ref class VMWrapper // Lua Wrapper for System::Type^
	{
	public:
		static cli::array<EngineAssembly^>^ GetAssemblies();

	public:
		static System::Type^ GetRuntimeType(System::Object^ object)
		{
			return object->GetType();
		}

		static String^ GetFullName(System::Type^ type)
		{
			return type->FullName;
		}

		static void PrintType(System::Object^ object)
		{
			Engine::Scripting::Logging::Log(object->GetType()->FullName);
		}

		static System::Object^ ToObject(DynValue^ userData)
		{
			DataType dataType = userData->Type;

			if (userData == nullptr || dataType == DataType::Function)
				return nullptr;

			System::Object^ defaultObject = userData->ToObject<System::Object^>();

			return userData->ToObject<System::Object^>();
		}

		static System::Object^ ToDerivate(System::Object^ object, System::String^ targetType)
		{
			System::Type^ toType = GetTypeByName(targetType);

			if (toType == nullptr)
				return DynValue::Nil;

			auto convertedType = System::Convert::ChangeType(object, toType);

			return UserData::Create(convertedType);
		}

		static System::Object^ ToDerivate(System::Object^ object)
		{
			if (object == nullptr)
				return DynValue::Nil;

			System::Type^ toType = object->GetType();

			if (toType == nullptr)
				return DynValue::Nil;

			auto convertedType = System::Convert::ChangeType(object, toType);

			return UserData::Create(convertedType);
		}

		static void AlterVMGlobals(MoonSharp::Interpreter::Table^ globals, String^ globalName, Object^ newObject)
		{
			globals[globalName] = newObject;
		}

		static void DerivateVMGlobal(MoonSharp::Interpreter::Script^& vm, String^ globalName)
		{
			System::Object^ object = vm->Globals[globalName];

			vm->Globals[globalName] = System::Convert::ChangeType(object, object->GetType());
		}

		static void UpdateVMGlobal(MoonSharp::Interpreter::Script^& vm, String^ globalName, Object^ newObject)
		{
			vm->Globals[globalName] = newObject;
		}

		static System::Type^ GetTypeByName(System::String^ typeName)
		{
			auto asms = GetAssemblies();
			System::Type^ currentType = nullptr;

			for each (auto assembly in asms)
			{
				currentType = assembly->GetTypeByName(typeName);

				if (currentType != nullptr)
					break;
			}

			return currentType;
		}

		static DynValue^ Derivate(System::Object^ object);
		static bool HasProperty(Engine::Internal::Components::GameObject^ object, String^ propertyName);
		static Engine::Scripting::AttributeManager^ GetAttributeManager(Engine::Internal::Components::GameObject^ object);
		static void SetProperty(Engine::Internal::Components::GameObject^ object, String^ propertyName, System::Object^ newValue);

		static Object^ GetProperty(Engine::Internal::Components::GameObject^ instance, String^ propertyName);
	};
}