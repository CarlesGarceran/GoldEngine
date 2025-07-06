#include "Macros.h"
#include "Includes.h"
#include "GlIncludes.h"
#include "../SDK.h"
#include "Window.h"
#include <Windows.h>

using namespace Engine;

void Window::InitializeExtensions()
{
    /*
    GLADloadproc loaderProc;
	if (!gladLoadGLLoader(loaderProc))
    {
        printf("Failed to initialize OpenGL loader!\n");
        return;
    }
    RLGL::rlLoadExtensions((void*)loaderProc);
    */
}

void Window::Boot()
{
    System::IO::File::Create("./Data/UserData/firstInit.asset")->Close();
}

bool Window::FirstTimeBoot()
{
    Singleton<Window^>::Create(this);
    HarmonyLib::Harmony^ harmony = gcnew HarmonyLib::Harmony("HarmonyInstance");
    Singleton<HarmonyLib::Harmony^>::Create(harmony);

    return !System::IO::File::Exists("./Data/UserData/firstInit.asset");
}