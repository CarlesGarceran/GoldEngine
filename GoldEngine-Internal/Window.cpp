#include "Macros.h"
#include "Includes.h"
#include "GlIncludes.h"
#include "../SDK.h"
#include "Window.h"
#include <Windows.h>
#include "imnotifications/ImNotify.h"
#include "imnodes/Wrapper/ImNodesWrapper.h"
#include "imgui/Fonts/fa_solid_900.h"

using namespace Engine;

void Engine::Window::OpenWindow(int width, int height, const char* name)
{
	EngineState::glInitialized = true;

	InitWindow(width, height, name);
	InitAudioDevice();
	SetFPS(60);

	RAYLIB::SetMasterVolume(1.0f);

	RLGL::rlglInit(width, height);
	RLGL::rlEnableDepthTest();

	InitializeExtensions();

	rlImGuiSetup(true);
	ImNodesContext* ctx = ImNodes::CreateContext();
	ImNodesEditorContext* editorCtx = ImNodes::EditorContextCreate();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.IniFilename = "Cfg/gui.cfg";

	{
		float baseFontSize = 16.0f;
		float iconFontSize = baseFontSize * 2.0f / 3.0f;

		static constexpr ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		iconsConfig.GlyphMinAdvanceX = iconFontSize;
		io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size, iconFontSize, &iconsConfig, iconsRanges);
	}

	ImGuiNET::Nodes::ImNodes::SetImGuiContext(IntPtr(ImGui::GetCurrentContext()));
	ImNodes::StyleColorsDark();
	ImGuiNET::Nodes::ImNodes::SetCurrentContext(IntPtr(ctx));
}

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

Engine::Window::Window()
{
	printConsole(" -- | GOLD ENGINE | --");
	printConsole(ENGINE_VERSION);

}

bool Window::FirstTimeBoot()
{
    Singleton<Window^>::Create(this);
    HarmonyLib::Harmony^ harmony = gcnew HarmonyLib::Harmony("HarmonyInstance");
    Singleton<HarmonyLib::Harmony^>::Create(harmony);

    return !System::IO::File::Exists("./Data/UserData/firstInit.asset");
}