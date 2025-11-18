#include "Includes.h"
#include "CastToNative.h"
#include "GlIncludes.h"
#include "LoggingAPI.h"
#include "EngineGC.h"

using namespace Engine::GC;

long long toMB(long long value)
{
	return ((value) / 1000 / 1000);
}

void EngineGC::Update()
{
	long long memRes = toMB(System::GC::GetTotalMemory(false));
	if (memRes >= GC_SIZE)
	{
		if (GC_TRIGGERED_LASTFRAME)
		{
			GC_TRIGGERED_LASTFRAME = false;
			printConsole(gcnew String(TextFormat("GC Memory Threshold Expanded from %d to %d!", GC_SIZE, GC_SIZE + GC_MEMORY_INCREMENT)));
			GC_SIZE += GC_MEMORY_INCREMENT;
			return;
		}

		printWarning(gcnew String(TextFormat("GC Memory Exceeded %d/%d!", memRes, GC_SIZE)));
		System::GC::Collect();

		GC_TRIGGERED_LASTFRAME = true;
	}
}