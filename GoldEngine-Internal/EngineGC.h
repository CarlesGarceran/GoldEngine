#pragma once

#define MAX_GC_SIZE 256       // MB - initial threshold
#define GC_MEMORY_INCREMENT 128 // MB - increase threshold if memory keeps growing

namespace Engine::GC
{
    private ref class EngineGC
    {
    private:
        // Tracks if a GC warning/collection was triggered last frame
        static bool GC_TRIGGERED_LASTFRAME = false;

        // Current memory threshold in MB
        static int GC_SIZE = MAX_GC_SIZE;

    public:
        // Called every frame to check memory usage
        static void Update();

        // Force a collection (blocking)
        static void Collect();

        // Wait for pending finalizers
        static void WaitForPendingFinalizers();
    };
}