#pragma once

#include <msclr/lock.h>

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

#define printConsole Engine::Scripting::Logging::Log
#define printError Engine::Scripting::Logging::LogError
#define printWarning Engine::Scripting::Logging::LogWarning
#define printDebug Engine::Scripting::Logging::LogDebug
#define print Engine::Scripting::Logging::LogCustom

namespace Engine::Scripting
{
	public ref class Log
	{
	public:
		int logType;
		String^ message;

		Log(int level, String^ mesg)
		{
			logType = level;
			message = mesg;
		}
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Logging abstract
	{
	private:
		static List<Log^>^ log = gcnew System::Collections::Generic::List<Engine::Scripting::Log^>();

	public:
		static void Log(String^ message);
		static void LogCustom(String^ header, String^ message);
		static void LogDebug(String^ message);
		static void LogWarning(String^ message);
		static void LogFatal(String^ message);
		static void LogError(String^ message);

		static void clearLogs()
		{
			#if PRODUCTION_BUILD
				printWarning("Cannot use clearLogs on a production build (game).");
			#else
			msclr::lock l(log);

			if (l.try_acquire(1000))
				log->Clear();
			else
				clearLogs();

			l.release();

			#endif
		}

		static System::Collections::Generic::List<Engine::Scripting::Log^>^ getLogs()
		{
			if (log != nullptr)
				return log;
			else
				return nullptr;
		}
	};
}