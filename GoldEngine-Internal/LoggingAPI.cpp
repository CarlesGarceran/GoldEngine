#include "SDK.h"
#include "LoggingAPI.h"
#include "LogFileReporter.h"

void Engine::Scripting::Logging::Log(String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		//TraceLog(LOG_INFO, CastStringToNative(message).c_str());
		Console::WriteLine("[INFO] " + message);
		log->Add(gcnew Engine::Scripting::Log(LOG_INFO, "[INFO] " + message));
	}
	else
	{
		Log(message); // recurse call
	}

	l.release();
}

void Engine::Scripting::Logging::LogCustom(String^ header, String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		Console::WriteLine(header + " " + message);
		log->Add(gcnew Engine::Scripting::Log(LOG_INFO, header + " " + message));
	}
	else
	{
		Log(message); // recurse call
	}

	l.release();
}

void Engine::Scripting::Logging::LogDebug(String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		//TraceLog(LOG_DEBUG, CastStringToNative(message).c_str());
		Console::WriteLine("[DEBUG] " + message);
		log->Add(gcnew Engine::Scripting::Log(LOG_DEBUG, "[DEBUG] " + message));
	}
	else
	{
		LogDebug(message); // recurse call
	}

	l.release();
}

void Engine::Scripting::Logging::LogWarning(String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		//TraceLog(LOG_WARNING, CastStringToNative(message).c_str());
		Console::WriteLine("[WARNING] " + message);
		log->Add(gcnew Engine::Scripting::Log(LOG_WARNING, "[WARNING] " + message));
	}
	else
	{
		LogWarning(message); // recurse call
	}

	l.release();
}

void Engine::Scripting::Logging::LogFatal(String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		log->Add(gcnew Engine::Scripting::Log(LOG_FATAL, "[FATAL] " + message));
		Engine::Utils::LogReporter::singleton->CloseThread();
		System::Threading::Thread::Sleep(1000);
		RAYLIB::TraceLog(LOG_FATAL, CastStringToNative(message).c_str());
	}
	else
	{
		LogFatal(message); // recurse call
	}

	l.release();
}

void Engine::Scripting::Logging::LogError(String^ message)
{
	msclr::lock l(log);

	if (l.try_acquire(1000))
	{
		//TraceLog(LOG_ERROR, CastStringToNative(message).c_str());
		Console::WriteLine("[ERROR] " + message);
		log->Add(gcnew Engine::Scripting::Log(LOG_ERROR, "[ERROR] " + message));
	}
	else
	{
		LogError(message); // recurse call
	}

	l.release();
}
