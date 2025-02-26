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

using namespace Engine::Scripting;
using namespace Engine::Lua::VM;

// VMWRAPPER \\

cli::array<EngineAssembly^>^ VMWrapper::GetAssemblies()
{
	return Engine::Managers::SceneManager::GetAssemblyManager()->ToArray();
}

DynValue^ VMWrapper::Derivate(System::Object^ object)
{
	return UserData::Create(System::Convert::ChangeType(object, object->GetType()));
}

bool VMWrapper::HasProperty(Engine::Internal::Components::GameObject^ object, String^ propertyName)
{
	if (object->GetType()->IsSubclassOf(Engine::EngineObjects::ScriptBehaviour::typeid))
	{
		Engine::Scripting::AttributeManager^ attribs = ((Engine::EngineObjects::ScriptBehaviour^)object)->attributes;

		if (attribs->hasAttribute(propertyName))
		{
			return true;
		}
	}

	return false;
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

void VMWrapper::SetProperty(Engine::Internal::Components::GameObject^ object, String^ propertyName, System::Object^ newValue)
{
	if (HasProperty(object, propertyName))
	{
		Engine::Scripting::AttributeManager^ attribs = ((Engine::EngineObjects::ScriptBehaviour^)object)->attributes;
		attribs->getAttribute(propertyName)->setValue(newValue);
	}
}

Object^ VMWrapper::GetProperty(Engine::Internal::Components::GameObject^ object, String^ propertyName)
{
	if (HasProperty(object, propertyName))
	{
		Engine::Scripting::AttributeManager^ attribs = ((Engine::EngineObjects::ScriptBehaviour^)object)->attributes;
		return attribs->getAttribute(propertyName)->getValue();
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

void LuaVM::RegisterGlobalFunctions()
{
	for each (auto asms in VMWrapper::GetAssemblies())
	{
		try
		{
			UserData::RegisterAssembly(asms->getLoadedAssembly(), true);
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
	RegisterGlobal("SharedInstance", SharedInstance::typeid);
	RegisterGlobal("VMWrap", VMWrapper::typeid);
	RegisterGlobal("Time", Engine::Scripting::Time::typeid);
	RegisterGlobal("Graphics", Engine::Internal::GraphicsWrapper::typeid);
	RegisterGlobal("Vector2", Engine::Components::Vector2::typeid);
	RegisterGlobal("Vector3", Engine::Components::Vector3::typeid);
	RegisterGlobal("Color", Engine::Components::Color::typeid);
	RegisterGlobal("Event", Engine::Scripting::Events::Event::typeid);

	// REGISTER DATAMODEL INSTANCES (workspace, gui)
	if (Singleton<Engine::Scripting::ObjectManager^>::Instantiated)
	{
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

	// CREATE CUSTOM LUA FUNCTIONS
	RegisterGlobal("HasProperty", gcnew System::Func<Engine::Internal::Components::GameObject^, String^, bool>(&VMWrapper::HasProperty));
	RegisterGlobal("GetAttributes", gcnew System::Func<Engine::Internal::Components::GameObject^, Engine::Scripting::AttributeManager^>(&VMWrapper::GetAttributeManager));
	RegisterGlobal("SetProperty", gcnew System::Action<Engine::Internal::Components::GameObject^, String^, Object^>(&VMWrapper::SetProperty));
	RegisterGlobal("GetProperty", gcnew System::Func<Engine::Internal::Components::GameObject^, String^, Object^>(&VMWrapper::GetProperty));
	RegisterGlobal("CastToClass", gcnew System::Func<System::Object^, System::String^, System::Object^>(&VMWrapper::ToDerivate));
	RegisterGlobal("ToDerivate", gcnew System::Func<System::Object^, System::Object^>(&VMWrapper::ToDerivate));
	//RegisterGlobal("VM", this->scriptState);
}

System::Collections::Generic::List<Type^>^ LuaVM::GetMoonSharpTypes(System::Reflection::Assembly^ a)
{
	List<Type^>^ result = gcnew System::Collections::Generic::List<Type^>();

	for each (Type ^ t in a->GetTypes())
	{
		if (t->GetCustomAttributes(MoonSharp::Interpreter::MoonSharpUserDataAttribute::typeid, false)->Length > 0)
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

void LuaVM::GenerateLuaBindings()
{
	String^ fileName = "./Bindings/Lua/GoldEngineBindings.lua";
	int index = fileName->LastIndexOf("/");

	Directory::CreateDirectory(fileName->Substring(0, index));

	String^ luaSrcFile = "--[[ GoldVM Binding Generator : Work In Progress ]]--\n\n";

	try
	{
		for each (auto asms in VMWrapper::GetAssemblies())
		{
			try
			{
				List<Type^>^ lua_proxy_types = GetMoonSharpTypes(asms->getLoadedAssembly());


				for each (Type ^ type in lua_proxy_types)
				{
					String^ fullName = type->Name;

					if (fullName->Contains("Proxy"))
						continue;

					luaSrcFile += "\n--[[ " + fullName + " CLASS DEFINITION ]]--\n";

					luaSrcFile += fullName + " = {}\n";

					auto members = type->GetMembers();
					for each (auto member in members)
					{
						if (member->MemberType == System::Reflection::MemberTypes::Field || member->MemberType == System::Reflection::MemberTypes::Property)
						{
							luaSrcFile += fullName + "." + member->Name + " = nil;\n";
						}
					}

					auto methods = type->GetMethods();
					for each (auto method in methods)
					{
						if (method->IsPublic)
						{
							luaSrcFile += "function " + fullName + "." + method->Name + "(";

							auto params = method->GetParameters();
							int length = params->Length;
							for (int x = 0; x < length; x++)
							{
								auto param = params[x];
								if (x < length - 1)
								{
									luaSrcFile += param->Name + "__" + param->ParameterType->Name + ",";
								}
								else
								{
									luaSrcFile += param->Name + "__" + param->ParameterType->Name + "";
								}
							}

							luaSrcFile += ")end\n";
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

		File::WriteAllText("./Bindings/Lua/GoldEngineBindings.lua", luaSrcFile);
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

				tempBuffer = CypherLib::DecryptFileContents(base64, ::passwd);
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

void LuaVM::WriteLuaCodeToFile(String^ src)
{
	System::IO::FileStream^ f = System::IO::File::Open(src, System::IO::FileMode::OpenOrCreate);
	System::IO::BinaryWriter^ bwriter = gcnew System::IO::BinaryWriter(f);
	System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream();

	memoryStream->CopyTo(f);

	System::IO::Compression::ZLibStream^ zlibStream = gcnew System::IO::Compression::ZLibStream(memoryStream, System::IO::Compression::CompressionMode::Compress);

	bwriter->Write(BINARY_HEADER);
	bwriter->Write(BYTECODE_VERSION);
	auto bytes = System::Text::Encoding::UTF32->GetBytes(CypherLib::EncryptFileContents(source, ::passwd));

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

				return CypherLib::DecryptFileContents(base64, ::passwd);
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