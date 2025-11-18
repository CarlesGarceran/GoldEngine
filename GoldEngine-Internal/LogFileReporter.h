#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace Engine::Scripting;

namespace Engine::Utils
{
	ref class LogReporter
	{
	public:
		static LogReporter^ singleton;

	private:
		TextWriter^ fileStream;
		List<Log^>^ lifetimeLogs;
		Threading::Thread^ newThread;
		static bool exitCodeCalled = false;


	public:
		LogReporter(String^ fileName)
		{
			singleton = this;

			String^ GUID = System::Guid::NewGuid().ToString();

			String^ name = GUID->Substring(0, GUID->IndexOf('-'));

			String^ currDate = System::DateTimeOffset::Now.ToUnixTimeMilliseconds().ToString();

			fileStream = File::CreateText(fileName + "/GoldEngine-" + currDate + "-" + name + ".log");
			lifetimeLogs = gcnew System::Collections::Generic::List<Log^>();

			fileStream->Flush();

			newThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &LogReporter::ThreadProcessing));
			newThread->IsBackground = true;
			newThread->Start();
		}

	private:
		void WriteContentsToFile(String^ message)
		{
			fileStream->Write(message);
			fileStream->Write("\n");

			fileStream->Flush();
		}

		void ThreadProcessing()
		{
			while (true)
			{
				if (exitCodeCalled)
					break;

				if (Logging::getLogs() == nullptr)
					continue;

				List<Engine::Scripting::Log^>^ logs = Logging::getLogs();

				if (logs == nullptr)
					continue;

				msclr::lock l(logs);
				
				if (l.try_acquire(1000)) 
				{
					if (logs != nullptr)
					{
						for each (Log ^ log in logs)
						{
							if (log != nullptr)
							{
								if (!lifetimeLogs->Contains(log))
								{
									lifetimeLogs->Add(log);
									if (log->message->Length > 0)
									{
										WriteContentsToFile(log->message);
									}
								}
							}
						}
					}
				}


				l.release();
			}
		}

	public:
		void CloseThread()
		{
			exitCodeCalled = true;

			if (newThread != nullptr)
				newThread->Join();

			fileStream->Close();
		}

	};
}