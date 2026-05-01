#include "SDK.h"
#include "SceneManager.h"
#include "Attribute.h"
#include "AttributeManager.h"
#include "LuaVM.h"
#include "VMWrapper.h"
#include "DataManager.h"
#include "Time.h"
#include "GraphicsWrapper.h"
#include "ManagedSignal.h"

#include "Objects/LuaScript.h"

#include "LuaAPI.h"

using namespace Engine::Scripting;
using namespace Engine::Lua::VM;

inline String^ MapToLuaType(System::Type^ type)
{
	if (type == nullptr)
		return "any";

	// Handle nullable<T>
	if (type->IsGenericType &&
		type->GetGenericTypeDefinition() == System::Nullable::typeid)
	{
		auto inner = type->GetGenericArguments()[0];
		return MapToLuaType(inner) + "|nil";
	}

	// Primitive mappings
	if (type == System::Void::typeid)
		return ""; // special case (no return)

	if (type == System::Boolean::typeid)
		return "boolean";

	if (type == System::String::typeid)
		return "string";

	if (type->IsPrimitive)
		return "number"; // covers Int32, Float, Double, etc.

	// Arrays
	if (type->IsArray)
	{
		auto elementType = type->GetElementType();
		return MapToLuaType(elementType) + "[]";
	}

	// Everything else = treat as Lua class type
	return type->Name;
}

inline void luas_dump(MoonSharp::Interpreter::DynValue^ dynValue)
{

}

void Engine::Lua::VM::LuaVM::LuaVM_LoadString(Object^ param)
{
	System::String^ source = safe_cast<System::String^>(param);

	try
	{
		this->value = scriptState->LoadString(source, scriptState->Globals, "GoldEngineLuaThread");
		auto co = scriptState->CreateCoroutine(this->value);

		try
		{
			co->Coroutine->Resume();
		}
		finally
		{
			delete co;
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

void Engine::Lua::VM::LuaVM::LuaVM_RunFunctionByName(Object^ functionName)
{
	System::String^ source = safe_cast<System::String^>(functionName);

	try
	{
		if (hasFunction(source))
		{
			MoonSharp::Interpreter::Closure^ func = (MoonSharp::Interpreter::Closure^)scriptState->Globals[source];
			func->Call();
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

void Engine::Lua::VM::LuaVM::LuaVM_RunFunctionByPointer(Object^ functionPointer)
{
	MoonSharp::Interpreter::DynValue^ source = safe_cast<MoonSharp::Interpreter::DynValue^>(functionPointer);

	try
	{
		auto co = scriptState->CreateCoroutine(source);
		
		try
		{
			co->Coroutine->Resume();
		}
		finally
		{
			delete co;
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}
}

void Engine::Lua::VM::LuaVM::DumpSource(String^ source)
{
	this->source = source;

	this->scriptState->Dump(
		this->scriptState->LoadString(
			source,
			this->scriptState->Globals,
			"GoldEngineCompilerThread"
		),
		this->bytecode
	);
}

// VMWRAPPER \\

cli::array<EngineAssembly^>^ VMWrapper::GetAssemblies()
{
	return Engine::Managers::SceneManager::GetAssemblyManager()->ToArray();
}

DynValue^ VMWrapper::Derivate(System::Object^ object)
{
	return UserData::Create(System::Convert::ChangeType(object, object->GetType()));
}

Engine::Scripting::AttributeManager^ VMWrapper::GetAttributeManager(Engine::Internal::Components::GameObject^ object)
{
	if (object->GetType()->IsSubclassOf(Engine::EngineObjects::ScriptBehaviour::typeid))
	{
		Engine::Scripting::AttributeManager^ attribs = ((Engine::EngineObjects::ScriptBehaviour^)object)->attributes;
		return attribs;
	}

	return nullptr;
}

// STATIC VM FUNCTIONS \\

LuaVM^ LuaVM::RequireOverride(System::Object^ luaSource)
{
	if (luaSource->GetType() != Engine::EngineObjects::LuaScript::typeid)
	{
		throw gcnew MoonSharp::Interpreter::ScriptRuntimeException("Cannot require an object that is not a LuaScript");
		return nullptr;
	}

	return ((Engine::EngineObjects::LuaScript^)luaSource)->virtualMachine;
}

// LUAVM \\

void RegisterChild(MoonSharp::Interpreter::Table^ tableRoot, String^ typeName, System::Object^ object, String^ moduleName)
{
	if (moduleName == "")
	{
		
	}
	else
	{

	}
}

bool AwaitDebuggerAttach(MoonSharp::VsCodeDebugger::MoonSharpVsCodeDebugServer^ server)
{
	System::Reflection::BindingFlags flags = System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic;
	System::Reflection::FieldInfo^ fieldInfo = server->GetType()->GetField("m_Current", flags);

	Object^ current = fieldInfo->GetValue(server);
	System::Reflection::FieldInfo^ prop = current->GetType()->GetField("m_Client__", flags);

	System::Diagnostics::Stopwatch^ stopWatch = gcnew System::Diagnostics::Stopwatch();
	stopWatch->Start();
	printConsole("Waiting for VSCode debugger");
	while (prop->GetValue(current) == nullptr)
	{
		System::Threading::Thread::Sleep(500);
		if (stopWatch->Elapsed.TotalSeconds > 60) return false;
	}
	stopWatch->Stop();
	printConsole("VSCode Debugger Attached");
	return true;
}

void LuaVM::AttachDebugger()
{
	MoonSharp::VsCodeDebugger::MoonSharpVsCodeDebugServer^ debugServer = gcnew MoonSharp::VsCodeDebugger::MoonSharpVsCodeDebugServer();
	debugServer->Start();
	debugServer->AttachToScript(this->scriptState, "DebugScript");

	bool attached = AwaitDebuggerAttach(debugServer);
	if (!attached)
		printError("Could not attach debugger to Script.");
}

void Engine::Lua::VM::LuaVM::RegisterScript(String^ source)
{
	ExecuteSource(source);
}

void Engine::Lua::VM::LuaVM::RegisterCoroutine(String^ source)
{
	ExecuteSourceCo(source);
}

bool Engine::Lua::VM::LuaVM::InvokeFunctionCo(String^ functionName)
{
	try
	{
		if (hasFunction(functionName))
		{
			System::Threading::Thread^ thread = gcnew System::Threading::Thread(
				gcnew System::Threading::ParameterizedThreadStart(
					this, &Engine::Lua::VM::LuaVM::LuaVM_RunFunctionByName
				)
			);

			thread->Start(functionName);
			return true;
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}

	return false;
}

bool Engine::Lua::VM::LuaVM::InvokeFunctionCo(MoonSharp::Interpreter::DynValue^ function)
{
	try
	{
		System::Threading::Thread^ thread = gcnew System::Threading::Thread(
			gcnew System::Threading::ParameterizedThreadStart(
				this, &Engine::Lua::VM::LuaVM::LuaVM_RunFunctionByPointer
			)
		);

		thread->Start(function);
		return true;
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
	return false;
}

bool Engine::Lua::VM::LuaVM::InvokeFunction(String^ functionName)
{
	try
	{
		if (hasFunction(functionName))
		{
			scriptState->Call(scriptState->Globals[functionName]);
			return true;
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}

	return false;
}

bool Engine::Lua::VM::LuaVM::InvokeFunction(MoonSharp::Interpreter::DynValue^ functionName)
{
	try
	{
		scriptState->Call(functionName);
		return true;
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}

	return false;
}

bool Engine::Lua::VM::LuaVM::InvokeFunction(String^ functionName, array<System::Object^>^ args)
{
	try
	{
		if (hasFunction(functionName))
		{
			scriptState->Call(scriptState->Globals[functionName], args);
			return true;
		}
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}

	return false;
}

bool Engine::Lua::VM::LuaVM::InvokeFunction(String^ functionName, List<System::Object^>^ args)
{
	try
	{
		if (hasFunction(functionName))
		{
			scriptState->Call(scriptState->Globals[functionName], args->ToArray());
			return true;
		}
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}

	return false;
}

System::Object^ Engine::Lua::VM::LuaVM::InvokeFunctionO(MoonSharp::Interpreter::DynValue^ functionName)
{
	try
	{
		return scriptState->Call(functionName);
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}

	return nullptr;
}

void Engine::Lua::VM::LuaVM::ExecuteSourceCo(String^ source)
{
	DumpSource(source);

	try
	{
		System::Threading::Thread^ thread = gcnew System::Threading::Thread(
			gcnew System::Threading::ParameterizedThreadStart(
				this, &Engine::Lua::VM::LuaVM::LuaVM_LoadString
			)
		);

		thread->Start(source);
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (MoonSharp::Interpreter::InterpreterException^ ex)
	{
		printError(ex->DecoratedMessage);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

void Engine::Lua::VM::LuaVM::ExecuteSource(String^ source)
{
	DumpSource(source);

	try
	{
		value = scriptState->DoString(source, scriptState->Globals, "GoldEngineMainThread");
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

void Engine::Lua::VM::LuaVM::ExecuteSource(String^ source, String^ friendlyName)
{
	DumpSource(source);

	try
	{
		value = scriptState->DoString(source, scriptState->Globals, friendlyName);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

inline void LuaVM::RegisterGlobal(String^ functionName, System::Type^ userData)
{
	scriptState->Globals[functionName] = userData;
}

inline void LuaVM::RegisterTable(String^ tableName, MoonSharp::Interpreter::Table^ table)
{
	scriptState->Globals[tableName] = table;
}

inline void LuaVM::RegisterGlobal(String^ functionName, System::Object^ userData)
{
	scriptState->Globals[functionName] = userData;
}

inline void LuaVM::RegisterModule(String^ moduleName)
{
	scriptState->Globals[moduleName] = {};
}

String^ GetAPIName(System::Type^ type)
{
	array<System::Object^>^ arr = nullptr;
	if ((arr = type->GetCustomAttributes(Engine::Attributes::LuaAPIAttribute::typeid, false))->Length > 0)
	{
		for each (auto t in arr)
		{
			if (t->GetType() == Engine::Attributes::LuaAPIAttribute::typeid)
				return ((Engine::Attributes::LuaAPIAttribute^)t)->globalName;
		}
	}
	return "";
}

void LuaVM::RegisterGlobalFunctions()
{
	for each (auto asms in VMWrapper::GetAssemblies())
	{
		try
		{
			if (asms->getLoadedAssembly() == nullptr)
				continue;

			UserData::RegisterAssembly(asms->getLoadedAssembly(), true);

			List<System::Type^> types = asms->getLoadedAssembly()->GetTypes();

			for each (System::Type ^ type in types)
			{
				if (type == System::Type::typeid)
					continue;

				for each (System::Object ^ T in type->GetCustomAttributes(Engine::Attributes::LuaAPIAttribute::typeid, false))
				{
					if (T == nullptr)
						break;

					Engine::Attributes::LuaAPIAttribute^ attrib = (Engine::Attributes::LuaAPIAttribute^)T;

					if (attrib == nullptr)
						break;

					if (attrib->isStatic)
					{
						if (attrib->globalName == "")
							RegisterGlobal(type->Name, type);
						else
							RegisterGlobal(attrib->globalName, type);
					}
				}
			}
		}
		catch (Exception^ ex)
		{
			printError(ex->Message);
			printError(ex->StackTrace);
		}
	}

	// REGISTER ENGINE CLASSES
	RegisterGlobal("Logging", Engine::Scripting::Logging::typeid);
	RegisterGlobal("Attribute", Engine::Scripting::Attribute::typeid);
	RegisterGlobal("DataManager", Engine::Internal::DataManager::typeid);
	RegisterGlobal("ObjectManager", Engine::Scripting::ObjectManager::singleton());
	RegisterGlobal("Input", Engine::Scripting::InputManager::typeid);
	RegisterGlobal("KeyCode", Engine::Scripting::KeyCodes::typeid);
	RegisterGlobal("MouseButton", Engine::Scripting::MouseButtons::typeid);
	RegisterGlobal("CursorStatus", Engine::Scripting::CursorStatus::typeid);
	RegisterGlobal("CursorVisibility", Engine::Scripting::CursorVisibility::typeid);
	RegisterGlobal("Gamepad", Engine::Scripting::Gamepad::typeid);
	RegisterGlobal("GamepadAxis", Engine::Scripting::GamepadAxis::typeid);
	RegisterGlobal("GamepadButtons", Engine::Scripting::GamepadButtons::typeid);
	RegisterGlobal("SharedInstance", SharedInstance::typeid);
	RegisterGlobal("VMWrap", VMWrapper::typeid);
	RegisterGlobal("Time", Engine::Scripting::Time::typeid);
	RegisterGlobal("Screen", Engine::Scripting::Screen::typeid);
	//RegisterGlobal("Graphics", Engine::Internal::GraphicsWrapper::typeid);
	RegisterGlobal("Vector2", Engine::Components::Vector2::typeid);
	RegisterGlobal("Vector3", Engine::Components::Vector3::typeid);
	RegisterGlobal("Quaternion", Engine::Components::Quaternion::typeid);
	RegisterGlobal("Color", Engine::Components::Color::typeid);
	RegisterGlobal("Event", Engine::Scripting::Events::Event::typeid);
	
	// REGISTER DATAMODEL INSTANCES (workspace, gui)
	if (Singleton<Engine::Scripting::ObjectManager^>::Instantiated)
	{
		RegisterGlobal("game", Engine::Scripting::ObjectManager::singleton()->GetDatamodel("workspace")->Parent);
		RegisterGlobal("workspace", Engine::Scripting::ObjectManager::singleton()->GetDatamodel("workspace"));
		RegisterGlobal("gui", Engine::Scripting::ObjectManager::singleton()->GetDatamodel("gui"));
		RegisterGlobal("lighting", Engine::Scripting::ObjectManager::singleton()->GetDatamodel("lighting"));
	}

	if (Singleton<Engine::Managers::SignalManager^>::Instantiated)
	{
		RegisterGlobal("SignalManager", Singleton<Engine::Managers::SignalManager^>::Instance);
	}

	// OVERRIDE LUA FUNCTIONS
	RegisterGlobal("print", gcnew System::Action<String^>(&Logging::Log));
	RegisterGlobal("warn", gcnew System::Action<String^>(&Logging::LogWarning));
	RegisterGlobal("error", gcnew System::Action<String^>(&Logging::LogError));
	RegisterGlobal("info", gcnew System::Action<String^>(&Logging::LogDebug));
	RegisterGlobal("require", gcnew System::Func<System::Object^, LuaVM^>(this, &LuaVM::RequireOverride));
	RegisterGlobal("log", gcnew System::Action<String^, String^>(&Logging::LogCustom));
	RegisterGlobal("wait", gcnew System::Action<double>(&Time::Wait));

	// CREATE CUSTOM LUA FUNCTIONS
	RegisterGlobal("GetAttributes", gcnew System::Func<Engine::Internal::Components::GameObject^, Engine::Scripting::AttributeManager^>(&VMWrapper::GetAttributeManager));
	RegisterGlobal("CastToClass", gcnew System::Func<System::Object^, System::String^, System::Object^>(&VMWrapper::ToDerivate));
	RegisterGlobal("ToDerivate", gcnew System::Func<System::Object^, System::Object^>(&VMWrapper::ToDerivate));
	//RegisterGlobal("VM", this->scriptState);
}

System::Collections::Generic::List<Type^>^ LuaVM::GetMoonSharpTypes(System::Reflection::Assembly^ a)
{
	List<Type^>^ result = gcnew System::Collections::Generic::List<Type^>();

	for each (Type ^ t in a->GetTypes())
	{
		if (t->GetCustomAttributes(Engine::Attributes::LuaAPIAttribute::typeid, false)->Length > 0)
		{
			result->Add(t);
		}
	}

	return result;
}


void LuaVM::ClearGlobals()
{
	this->scriptState->Globals->Clear();
}

void RemapFunctions(String^% luaSrcFile, System::Type^ type, String^ apiName)
{
	auto methods = type->GetMethods();

	// Group by Name + Static flag
	System::Collections::Generic::Dictionary<
		String^,
		System::Collections::Generic::List<System::Reflection::MethodInfo^>^
	>^ methodGroups =
		gcnew System::Collections::Generic::Dictionary<
		String^,
		System::Collections::Generic::List<System::Reflection::MethodInfo^>^>();

	for each (auto method in methods)
	{
		if (!method->IsPublic)
			continue;

		String^ key = method->Name + (method->IsStatic ? "_STATIC" : "_INSTANCE");

		if (!methodGroups->ContainsKey(key))
			methodGroups[key] =
			gcnew System::Collections::Generic::List<System::Reflection::MethodInfo^>();

		methodGroups[key]->Add(method);
	}

	for each (auto pair in methodGroups)
	{
		auto overloads = pair.Value;
		auto firstMethod = overloads[0];

		String^ methodName = firstMethod->Name;
		bool isStatic = firstMethod->IsStatic;

		// ===== LuaLS Annotations =====
		for (int o = 0; o < overloads->Count; o++)
		{
			auto methodInfo = overloads[o];
			auto parameters = methodInfo->GetParameters();
			String^ returnType = MapToLuaType(methodInfo->ReturnType);

			if (o == 0)
				luaSrcFile += "---@field " + methodName + " fun(";
			else
				luaSrcFile += "---@overload fun(";

			if (!isStatic)
				luaSrcFile += "self: " + apiName;

			for (int i = 0; i < parameters->Length; i++)
			{
				if (!isStatic || i > 0)
					luaSrcFile += ", ";

				luaSrcFile += parameters[i]->Name + ":" +
					MapToLuaType(parameters[i]->ParameterType);
			}

			luaSrcFile += ")";

			if (returnType != "")
				luaSrcFile += ":" + returnType;

			luaSrcFile += "\n";
		}

		// ===== Runtime Stub =====
		if (isStatic)
			luaSrcFile += "function " + apiName + "." + methodName + "(";
		else
			luaSrcFile += "function " + apiName + ":" + methodName + "(";

		auto stubParams = firstMethod->GetParameters();

		for (int i = 0; i < stubParams->Length; i++)
		{
			luaSrcFile += stubParams[i]->Name;
			if (i < stubParams->Length - 1)
				luaSrcFile += ",";
		}

		luaSrcFile += ") end\n\n";
	}
}

System::Object^ GetDefaultValue(System::Type^ type)
{
	return type->IsValueType ? System::Activator::CreateInstance(type) : nullptr;
}

void RemapConstructors(String^% luaSrcFile, System::Type^ type, String^ apiName)
{
	auto constructors = type->GetConstructors();

	for each (auto constructor in constructors)
	{
		if (constructor->IsPrivate)
			continue;

		luaSrcFile += "---@return " + apiName + "\n";
		luaSrcFile += "function " + apiName + ".new(";

		auto params = constructor->GetParameters();

		for (int i = 0; i < params->Length; i++)
		{
			luaSrcFile += params[i]->Name;

			if (i < params->Length - 1)
				luaSrcFile += ",";
		}

		luaSrcFile += ")\n";
		luaSrcFile += "local self = setmetatable({}, " + apiName + ")\n";

		// Optional: initialize fields to nil only
		auto members = type->GetMembers();
		for each (auto member in members)
		{
			if (member->MemberType ==
				System::Reflection::MemberTypes::Field)
			{
				auto fieldInfo =
					(System::Reflection::FieldInfo^)member;

				if (!fieldInfo->IsStatic)
					luaSrcFile += "self." + fieldInfo->Name + " = nil\n";
			}
		}

		luaSrcFile += "return self\n";
		luaSrcFile += "end\n\n";
	}
}

void LuaVM::GenerateLuaBindings()
{
	String^ fileName = "./Bindings/Lua/GoldEngineBindings.lua";
	int index = fileName->LastIndexOf("/");

	Directory::CreateDirectory(fileName->Substring(0, index));

	String^ luaSrcFile =
		"--[[\n"
		"  GoldVM Lua Binding Generator v2\n"
		"  Auto-generated bindings for LuaLS and runtime\n"
		"  Supports:\n"
		"    • Classes and inheritance\n"
		"    • Static and instance fields\n"
		"    • Constructors with return typing\n"
		"    • Methods with overloads\n"
		"    • Typed globals (_G.script, _G.workspace, etc.)\n"
		"  WARNING: Do not manually edit, regenerated automatically\n"
		"]]--\n\n";

	try
	{
		for each (auto asms in VMWrapper::GetAssemblies())
		{
			try
			{
				List<Type^>^ lua_proxy_types = GetMoonSharpTypes(asms->getLoadedAssembly());

				for each (Type ^ type in lua_proxy_types)
				{
					String^ apiName = GetAPIName(type);
					if (apiName == "")
						apiName = type->Name;

					if (apiName->Contains("Proxy"))
						continue;

					luaSrcFile += "\n--[[ " + apiName + " CLASS DEFINITION ]]--\n";

					// ---------------------------
					// 1️⃣ CLASS HEADER
					// ---------------------------
					String^ baseType = "";
					if (type->BaseType != nullptr &&
						type->BaseType != System::Object::typeid)
					{
						baseType = GetAPIName(type->BaseType);
						if (baseType == "")
							baseType = type->BaseType->Name;
					}

					luaSrcFile += "---@class " + apiName;
					if (baseType != "")
						luaSrcFile += " : " + baseType;
					luaSrcFile += "\n";

					// ---------------------------
					// 2️⃣ FIELDS (STATIC + INSTANCE)
					// ---------------------------
					auto members = type->GetMembers();
					for each (auto member in members)
					{
						if (member->MemberType == System::Reflection::MemberTypes::Field)
						{
							auto fieldInfo = (System::Reflection::FieldInfo^)member;
							String^ fieldType = MapToLuaType(fieldInfo->FieldType);

							if (fieldInfo->IsStatic) continue;
							luaSrcFile += "---@field ";
							luaSrcFile += fieldInfo->Name + " " + fieldType + "\n";
						}
						else if (member->MemberType == System::Reflection::MemberTypes::Property)
						{
							auto propInfo = (System::Reflection::PropertyInfo^)member;
							String^ propType = MapToLuaType(propInfo->PropertyType);
							
							if (propInfo->GetMethod != nullptr &&
								propInfo->GetMethod->IsStatic)
								continue;

							luaSrcFile += "---@field " +
								propInfo->Name + " " +
								MapToLuaType(propInfo->PropertyType) + "\n";
						}
					}

					for each (auto member in members)
					{
						if (member->MemberType == System::Reflection::MemberTypes::Field)
						{
							auto fieldInfo = (System::Reflection::FieldInfo^)member;
							if (!fieldInfo->IsStatic) continue;

							luaSrcFile += "---@type " +
								MapToLuaType(fieldInfo->FieldType) + "\n";

							luaSrcFile += apiName + "." +
								fieldInfo->Name + " = nil\n\n";
						}
						else if (member->MemberType == System::Reflection::MemberTypes::Property)
						{
							auto propInfo = (System::Reflection::PropertyInfo^)member;

							if (propInfo->GetMethod == nullptr ||
								!propInfo->GetMethod->IsStatic)
								continue;

							luaSrcFile += "---@type " +
								MapToLuaType(propInfo->PropertyType) + "\n";

							luaSrcFile += apiName + "." +
								propInfo->Name + " = nil\n\n";
						}
					}

					// ---------------------------
					// 3️⃣ TABLE + METATABLE
					// ---------------------------
					luaSrcFile += apiName + " = {}\n";
					luaSrcFile += apiName + ".__index = " + apiName + "\n\n";
					
					// ---------------------------
					// 4️⃣ CONSTRUCTORS
					// ---------------------------
					RemapConstructors(luaSrcFile, type, apiName);

					// ---------------------------
					// 5️⃣ METHODS
					// ---------------------------
					RemapFunctions(luaSrcFile, type, apiName);
				}
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
				printError(ex->StackTrace);
			}
		}


		// ---------------------------
		// 6️⃣ GLOBAL SINGLETONS
		// ---------------------------
		luaSrcFile += "---@type LuaScript\n_G.script = LuaScript.new()\n";
		luaSrcFile += "---@type GameObject\n_G.workspace = GameObject.new()\n";
		luaSrcFile += "---@type GameObject\n_G.game = GameObject.new()\n";
		luaSrcFile += "---@type AttributeManager\n_G.attributes = AttributeManager.new()\n";

		File::WriteAllText(fileName, luaSrcFile);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}
}

void LuaVM::ReadLuaCodeFromFile(String^ src)
{
	System::IO::FileStream^ f = System::IO::File::Open(src, System::IO::FileMode::OpenOrCreate);
	System::IO::BinaryReader^ breader = gcnew System::IO::BinaryReader(f);
	System::IO::MemoryStream^ memoryStream;
	System::IO::Compression::ZLibStream^ zlibStream;

	String^ header = breader->ReadString();
	int version = breader->ReadInt32();
	int len = breader->ReadInt32();


	if (header->Equals(BINARY_HEADER))
	{
		if (version == BYTECODE_VERSION)
		{
			try
			{
				auto bytes = breader->ReadBytes(len);
				memoryStream = gcnew System::IO::MemoryStream(bytes, false);
				zlibStream = gcnew System::IO::Compression::ZLibStream(memoryStream, System::IO::Compression::CompressionMode::Decompress);

				array<unsigned char>^ decompressedData = gcnew array<unsigned char>(len);
				zlibStream->Read(decompressedData, 0, len);

				String^ base64 = System::Text::Encoding::UTF32->GetString(decompressedData);

				tempBuffer = Engine::Encryption::CypherLib::DecryptString(base64, ::passwd);
			}
			catch (Exception^ ex)
			{
				printError("Failed loading script source");
				printError(ex->Message);
			}
		}
		else
		{
			printError("Lua version mismatch\n");
		}
	}
	else
	{
		printError("Lua header mismatch\n");
	}
}

Engine::Lua::VM::LuaVM::LuaVM()
{
	tempBuffer = "";
	MoonSharp::Interpreter::Script::DefaultOptions->CheckThreadAccess = false;

	scriptState = gcnew MoonSharp::Interpreter::Script(CoreModules::Preset_Complete);
	scriptState->Options->CheckThreadAccess = false;

	bytecode = gcnew System::IO::MemoryStream();

	RegisterGlobalFunctions();
}

void LuaVM::WriteLuaCodeToFile(String^ src)
{
	System::IO::FileStream^ f = System::IO::File::Open(src, System::IO::FileMode::OpenOrCreate);
	System::IO::BinaryWriter^ bwriter = gcnew System::IO::BinaryWriter(f);
	System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream();

	memoryStream->CopyTo(f);

	System::IO::Compression::ZLibStream^ zlibStream = gcnew System::IO::Compression::ZLibStream(memoryStream, System::IO::Compression::CompressionMode::Compress);

	bwriter->Write(BINARY_HEADER);
	bwriter->Write(BYTECODE_VERSION);
	auto bytes = System::Text::Encoding::UTF32->GetBytes(Engine::Encryption::CypherLib::EncryptString(source, ::passwd));

	zlibStream->Write(bytes, 0, bytes->Length);
	zlibStream->Flush();
	zlibStream->Close();

	auto compressedData = memoryStream->ToArray();
	memoryStream->Close();

	bwriter->Write(bytes->Length);
	bwriter->Write(compressedData);

	bwriter->Close();
}


String^ LuaVM::ReadFromFile(String^ src)
{
	System::IO::FileStream^ f = System::IO::File::Open(src, System::IO::FileMode::OpenOrCreate);
	System::IO::BinaryReader^ breader = gcnew System::IO::BinaryReader(f);
	System::IO::MemoryStream^ memoryStream;
	System::IO::Compression::ZLibStream^ zlibStream;

	String^ header = breader->ReadString();
	int version = breader->ReadInt32();
	int len = breader->ReadInt32();


	if (header->Equals(BINARY_HEADER))
	{
		if (version == BYTECODE_VERSION)
		{
			try
			{
				auto bytes = breader->ReadBytes(len);
				memoryStream = gcnew System::IO::MemoryStream(bytes, false);
				zlibStream = gcnew System::IO::Compression::ZLibStream(memoryStream, System::IO::Compression::CompressionMode::Decompress);

				array<unsigned char>^ decompressedData = gcnew array<unsigned char>(len);
				zlibStream->Read(decompressedData, 0, len);

				String^ base64 = System::Text::Encoding::UTF32->GetString(decompressedData);

				return Engine::Encryption::CypherLib::DecryptString(base64, ::passwd);
			}
			catch (Exception^ ex)
			{
				printError("Failed loading script source");
				printError(ex->Message);
			}
		}
		else
		{
			printError("Lua version mismatch\n");
		}
	}
	else
	{
		printError("Lua header mismatch\n");
	}

	return "";
}