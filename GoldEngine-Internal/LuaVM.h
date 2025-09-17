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

		System::IO::Stream^ bytecode;

	public:
		LuaVM();

	public:
		auto GetGlobals() { return scriptState->Globals; }
		auto GetRegistry() { return scriptState->Registry; }
		MoonSharp::Interpreter::Script^ GetScriptState() { return scriptState; }

		void LoadSource(String^ source)
		{
			DumpSource(source);
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

		generic <class T>
		void RegisterGlobalFunction(String^ functionName, T method)
		{
			scriptState->Globals[functionName] = method;
		}

	public:
		void RegisterScript(String^ source);
		void RegisterCoroutine(String^ source);

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
		bool InvokeFunctionCo(String^ functionName);
		bool InvokeFunctionCo(MoonSharp::Interpreter::DynValue^ function);

		bool InvokeFunction(String^ functionName);
		bool InvokeFunction(MoonSharp::Interpreter::DynValue^ functionName);
		bool InvokeFunction(String^ functionName, array<System::Object^>^ args);
		bool InvokeFunction(String^ functionName, List<System::Object^>^ args);
		System::Object^ InvokeFunctionO(MoonSharp::Interpreter::DynValue^ functionName);

	private:
		void LuaVM_LoadString(Object^ source);
		void LuaVM_RunFunctionByName(Object^ functionName);
		void LuaVM_RunFunctionByPointer(Object^ functionPointer);

		void DumpSource(String^ source);
		void ExecuteSourceCo(String^ source);
		void ExecuteSource(String^ source);
		void ExecuteSource(String^ source, String^ friendlyName);

		DynValue^ RunScript(String^ source)
		{
			DumpSource(source);

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
			DumpSource(source);

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