#pragma once

using namespace System;
using namespace MoonSharp::Interpreter;

namespace Engine::Lua::VM
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		public ref class LuaVM
	{
	public:
		String^ BINARY_HEADER = "GoldVM";
		int BYTECODE_VERSION = 0x72;
		String^ tempBuffer;

	private:
		MoonSharp::Interpreter::Script^ scriptState;
		String^ source;
		DynValue^ value;

	public:
		LuaVM()
		{
			tempBuffer = "";
			MoonSharp::Interpreter::Script::DefaultOptions->CheckThreadAccess = false;

			scriptState = gcnew MoonSharp::Interpreter::Script(CoreModules::Preset_Complete);
			scriptState->Options->CheckThreadAccess = false;

			RegisterGlobalFunctions();
		}

	public:
		auto GetGlobals()
		{
			return scriptState->Globals;
		}

	public:
		MoonSharp::Interpreter::Script^ GetScriptState()
		{
			return scriptState;
		}

	public:
		void LoadSource(String^ source)
		{
			this->source = source;
		}


	public:
		void WriteLuaCodeToFile(String^ src);
		void ReadLuaCodeFromFile(String^ src);

		String^ ReadFromFile(String^ src);

		String^ LoadLuaCodeFromFile(String^ src)
		{
			return ReadFromFile(src);
		}

	private:
		LuaVM^ RequireOverride(System::Object^ luaSource);

		static System::Collections::Generic::List<Type^>^ GetMoonSharpTypes(System::Reflection::Assembly^ a);

	public:
		static void GenerateLuaBindings();

		void RegisterGlobalFunctions();
		void ClearGlobals();

		inline void RegisterModule(String^ moduleName);
		inline void RegisterGlobal(String^ functionName, System::Type^ userData);
		inline void RegisterTable(String^ tableName, MoonSharp::Interpreter::Table^ table);
		inline void RegisterGlobal(String^ functionName, System::Object^ userData);

		void AttachDebugger();

		generic <class T>
		void RegisterGlobalFunction(String^ functionName, System::Func<T>^ func)
		{
			scriptState->Globals[functionName] = func;
		}

		generic <class T>
		void RegisterGlobalFunction(String^ functionName, Action<T>^ method)
		{
			scriptState->Globals[functionName] = method;
		}

	public:
		void RegisterScript(String^ source)
		{
			ExecuteSource(source);
		}

	private:
		bool hasFunction(String^ data)
		{
			return scriptState->Globals[data] != nullptr;
		}

		bool hasFunction(MoonSharp::Interpreter::DynValue^ data)
		{
			return scriptState->Globals[data] != nullptr;
		}

	public:
		bool InvokeFunction(String^ functionName)
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

		bool InvokeFunction(MoonSharp::Interpreter::DynValue^ functionName)
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

		bool InvokeFunction(String^ functionName, array<System::Object^>^ args)
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

		bool InvokeFunction(String^ functionName, List<System::Object^>^ args)
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

		System::Object^ InvokeFunctionO(MoonSharp::Interpreter::DynValue^ functionName)
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

	private:
		void ExecuteSource(String^ source)
		{
			this->source = source;

			try
			{
				value = scriptState->DoString(source, scriptState->Globals, "GoldEngineLuaThread");
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
			}
		}

		void ExecuteSource(String^ source, String^ friendlyName)
		{
			this->source = source;
			try
			{
				value = scriptState->DoString(source, scriptState->Globals, friendlyName);
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
			}
		}

		DynValue^ RunScript(String^ source)
		{
			this->source = source;

			try
			{
				return scriptState->DoString(source, scriptState->Globals, "GoldEngineLuaThread");
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
				return nullptr;
			}
		}

		DynValue^ RunScript(String^ source, String^ friendlyName)
		{
			this->source = source;

			try
			{
				return scriptState->DoString(source, scriptState->Globals, friendlyName);
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
				return nullptr;
			}
		}
	};
}