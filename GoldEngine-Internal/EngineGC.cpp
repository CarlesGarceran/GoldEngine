#include "Includes.h"
#include "CastToNative.h"
#include "GlIncludes.h"
#include "LoggingAPI.h"
#include "EngineGC.h"

using namespace Engine::GC;

// Convert bytes to MB (binary MB)
inline long long toMB(long long bytes)
{
    return bytes / 1024 / 1024;
}

// ------------------- EngineGC Implementation -------------------
void EngineGC::Update()
{
    long long memMB = toMB(System::GC::GetTotalMemory(false));

    if (memMB >= GC_SIZE)
    {
        System::GC::Collect(0, GCCollectionMode::Default, false);

        if (!GC_TRIGGERED_LASTFRAME)
        {
            printWarning(gcnew String(TextFormat(
                "GC Memory exceeded threshold: %lld/%d MB (Gen0 collection triggered)",
                memMB, GC_SIZE
            )));
            GC_TRIGGERED_LASTFRAME = true;
        }
        else
        {
            GC_SIZE += GC_MEMORY_INCREMENT;
            printConsole(gcnew String(TextFormat(
                "GC Memory Threshold increased to %d MB", GC_SIZE
            )));
        }
    }
    else
    {
        GC_TRIGGERED_LASTFRAME = false;
    }
}

void EngineGC::Collect()
{
    System::GC::Collect();
    System::GC::WaitForPendingFinalizers();
}

void EngineGC::WaitForPendingFinalizers()
{
    System::GC::WaitForPendingFinalizers();
}