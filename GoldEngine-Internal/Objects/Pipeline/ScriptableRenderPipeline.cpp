#include "../../SDK.h"
#include "../../Screen.h"
#include "ScriptableRenderPipeline.hpp"

using namespace Engine::Scripting;
using namespace Engine::Components;

void Engine::Render::UnloadRenderTextureDepthTex(RenderTexture2D target)
{
	if (target.id > 0)
	{
		// Color texture attached to FBO is deleted
		RLGL::rlUnloadTexture(target.texture.id);
		RLGL::rlUnloadTexture(target.depth.id);

		// NOTE: Depth texture is automatically
		// queried and deleted before deleting framebuffer
		RLGL::rlUnloadFramebuffer(target.id);
	}
}

void Engine::Render::unloadCameraData(cameraData* data)
{
	delete data;
}

Engine::Render::ScriptableRenderPipeline::ScriptableRenderPipeline()
{
	Engine::Scripting::Logging::Log("Creating new ScriptableRenderPipeline");

	Singleton<ScriptableRenderPipeline^>::Create(this);
	Singleton<HarmonyLib::Harmony^>::Instance->UnpatchAll("HarmonyInstance");

	this->height = Engine::Scripting::Screen::Height;
	this->width = Engine::Scripting::Screen::Width;

	RAYLIB::RenderTexture renderTexture = LoadRenderTextureDepthTex(Engine::Scripting::Screen::Width, Engine::Scripting::Screen::Height);
	framebufferTexturePtr = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(renderTexture, &UnloadRenderTextureDepthTex);
	RAYLIB::RenderTexture rt2 = LoadRenderTextureDepthTex(Engine::Scripting::Screen::Width, Engine::Scripting::Screen::Height);
	savedTexPtr = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(rt2, &UnloadRenderTextureDepthTex);

	RAYLIB::Shader depthShader = RAYLIB::LoadShader("Data/Engine/Shaders/base.vs", "Data/Engine/Shaders/depth.frag");
	depthShaderPtr = new Engine::Native::EnginePtr<RAYLIB::Shader>(depthShader, &RAYLIB::UnloadShader);

	effects = gcnew Collections::Generic::List<ScriptableEffect^>();
}

RAYLIB::RenderTexture* Engine::Render::ScriptableRenderPipeline::getFrameBuffer()
{
	return &framebufferTexturePtr->getInstance();
}