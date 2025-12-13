#include "../../SDK.h"
#include "../../Screen.h"
#include "ScriptableRenderPipeline.hpp"
#include "../UI/RenderSurface3D.h"
#include "../../LuaVM.h"
#include "../EditorTools/CodeEditor.h"
#include "../imgui/FileExplorer/filedialog.h"
#include "../EditorWindow.h"

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

void Engine::Render::CallImGuizmoRender(Engine::Window^ windowPtr)
{
#if(!PRODUCTION_BUILD)
	((EditorWindow^)windowPtr)->DrawImGuizmo();
#endif
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

bool Engine::Render::ScriptableRenderPipeline::isInRenderSurface(GameObject^ gameObject)
{
	if (gameObject == nullptr)
		return false;

	if (gameObject->GetType()->IsSubclassOf(Engine::EngineObjects::Surface::RenderSurface::typeid))
		return true;
	else
		if (gameObject->transform->parent == nullptr)
			return false;
		else
			return isInRenderSurface(gameObject->Parent);
}

RenderTexture2D Engine::Render::ScriptableRenderPipeline::LoadRenderTextureDepthTex(int width, int height)
{
	RenderTexture2D target = { 0 };

	target.id = RLGL::rlLoadFramebuffer(); // Load an empty framebuffer

	if (target.id > 0)
	{
		RLGL::rlEnableFramebuffer(target.id);

		// Create color texture (default to RGBA)
		target.texture.id = RLGL::rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
		target.texture.width = width;
		target.texture.height = height;
		target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		target.texture.mipmaps = 1;

		// Create depth texture buffer (instead of raylib default renderbuffer)
		target.depth.id = RLGL::rlLoadTextureDepth(width, height, false);
		target.depth.width = width;
		target.depth.height = height;
		target.depth.format = PIXELFORMAT_UNCOMPRESSED_R32;
		target.depth.mipmaps = 1;

		// Attach color texture and depth texture to FBO
		RLGL::rlFramebufferAttach(target.id, target.texture.id, RLGL::RL_ATTACHMENT_COLOR_CHANNEL0, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);
		RLGL::rlFramebufferAttach(target.id, target.depth.id, RLGL::RL_ATTACHMENT_DEPTH, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);

		// Check if fbo is complete with attachments (valid)
		if (RLGL::rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

		RLGL::rlDisableFramebuffer();
	}
	else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

	return target;
}

void Engine::Render::ScriptableRenderPipeline::ExecuteRenderWorkflow(Engine::Window^ windowHandle, Engine::Management::Scene^ scene)
{
	auto c = gcnew Engine::Components::Color(scene->skyColor);
	RAYLIB::Color clearColor = c->toNative();

	PreFirstPassRender(scene); // FIRSTPASS
	PostFirstPassRender();

	{
		CreateTexture();

		BeginTextureMode(this->framebufferTexturePtr->getInstance());

		OnRenderBegin(); // BEGIN

		ClearBackground(clearColor);

		RLGL::rlEnableDepthTest();

		PreRenderFrame(); // PRE FRAME
		{
			Engine::EngineObjects::Camera^ camera = Engine::Scripting::ObjectManager::singleton()->GetMainCamera(true);

			int currentLayer = 0;

			if (camera == nullptr)
				goto RENDER_END;

			BeginMode3D(((Engine::EngineObjects::Native::NativeCamera3D*)camera->get())->get());

			render(currentLayer, scene);

			EndMode3D();

		RENDER_END:

			renderGUI(currentLayer, scene);
		}
		PostRenderFrame();
		RLGL::rlDisableDepthTest();
		EndTextureMode();

		for each(ScriptableEffect ^ effect in effects)
		{
			effect->SetFramebuffer(&framebufferTexturePtr->getInstance());
			effect->SetDepth(&framebufferTexturePtr->getInstance().depth);
			effect->SetTexture(&framebufferTexturePtr->getInstance().texture);

			BeginTextureMode(savedTexPtr->getInstance());
			{
				ClearBackground(clearColor);

				if (effect->ManualRendering())
				{
					effect->OnEffectApply(&framebufferTexturePtr->getInstance());
				}
				else
				{
					effect->OnEffectBegin();

					RAYLIB::Rectangle target;
					target.x = 0;
					target.y = 0;
					target.width = Engine::Scripting::Screen::Width;
					target.height = -Engine::Scripting::Screen::Height;

					DrawTextureRec(framebufferTexturePtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });

					effect->OnEffectEnd();
				}
			}
			EndTextureMode();

			// RESET

			BeginTextureMode(framebufferTexturePtr->getInstance());
			{
				ClearBackground(clearColor);

				RAYLIB::Rectangle target;
				target.x = 0;
				target.y = 0;
				target.width = Engine::Scripting::Screen::Width;
				target.height = -Engine::Scripting::Screen::Height;

				DrawTextureRec(savedTexPtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });
			}
			EndTextureMode();
		}

		OnRenderEnd();

		RAYLIB::BeginDrawing();

		rlImGuiBegin();

		for each(GameObject ^ obj in scene->GetRenderQueue())
		{
			if (obj != nullptr)
			{
				if (obj != nullptr)
				{
					obj->GameDrawImGUI();
				}
			}
		}

#ifndef PRODUCTION_BUILD
		if (windowHandle->GetType() == EditorWindow::typeid && EngineState::PlayMode)
		{
			auto viewPort = ImGui::GetMainViewport();
			ImGui::DockSpaceOverViewport(viewPort->ID, viewPort, ImGuiDockNodeFlags_PassthruCentralNode);
			((EditorWindow^)windowHandle)->DrawToolbar();
		}
#endif


		auto clearColor = gcnew Engine::Components::Color(scene->skyColor);
		ClearBackground(clearColor->toNative());

		RAYLIB::Rectangle target;
		target.x = 0;
		target.y = 0;
		target.width = Engine::Scripting::Screen::Width;
		target.height = -Engine::Scripting::Screen::Height;

		DrawTextureRec(framebufferTexturePtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });

		rlImGuiEnd();

		RAYLIB::EndDrawing();
	}

	PostRenderFrame();

	RLGL::rlReloadTextureUnits();
}

void Engine::Render::ScriptableRenderPipeline::ExecuteRenderWorkflow_Editor(Engine::Window^ windowHandle, Engine::Management::Scene^ scene)
{
	auto c = gcnew Engine::Components::Color(scene->skyColor);
	RAYLIB::Color clearColor = c->toNative();

	PreFirstPassRender(scene); // FIRSTPASS
	PostFirstPassRender();

	BeginDrawing();
	{
		CreateTexture();

		BeginTextureMode(this->framebufferTexturePtr->getInstance());

		OnRenderBegin(); // BEGIN

		ClearBackground(clearColor);

		RLGL::rlEnableDepthTest();

		PreRenderFrame(); // PRE FRAME
		{
			Engine::EngineObjects::Camera^ camera = Engine::Scripting::ObjectManager::singleton()->GetMainCamera(false);

			int currentLayer = 0;

			if (camera == nullptr)
				goto RENDER_END;

			BeginMode3D(((Engine::EngineObjects::Native::NativeCamera3D*)camera->get())->get());

			render(currentLayer, scene);

			EndMode3D();

		RENDER_END:

			renderGUI(currentLayer, scene);
		}
		PostRenderFrame();
		RLGL::rlDisableDepthTest();
		EndTextureMode();

		for each (ScriptableEffect ^ effect in effects)
		{
			effect->SetFramebuffer(&framebufferTexturePtr->getInstance());
			effect->SetDepth(&framebufferTexturePtr->getInstance().depth);
			effect->SetTexture(&framebufferTexturePtr->getInstance().texture);

			BeginTextureMode(savedTexPtr->getInstance());
			{
				ClearBackground(clearColor);

				if (effect->ManualRendering())
				{
					effect->OnEffectApply(&framebufferTexturePtr->getInstance());
				}
				else
				{
					effect->OnEffectBegin();

					RAYLIB::Rectangle target;
					target.x = 0;
					target.y = 0;
					target.width = Engine::Scripting::Screen::Width;
					target.height = -Engine::Scripting::Screen::Height;

					DrawTextureRec(framebufferTexturePtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });
					
					effect->OnEffectEnd();
				}
			}
			EndTextureMode();

			// RESET

			BeginTextureMode(framebufferTexturePtr->getInstance());
			{
				ClearBackground(clearColor);

				RAYLIB::Rectangle target;
				target.x = 0;
				target.y = 0;
				target.width = Engine::Scripting::Screen::Width;
				target.height = -Engine::Scripting::Screen::Height;

				DrawTextureRec(savedTexPtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });
			}
			EndTextureMode();
		}

		OnRenderEnd();

		rlImGuiBegin();

		ImGui::Begin("DemoVer", (bool*)true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
		{
			ImGui::SetWindowSize(ImVec2(285, 20), 0);
			ImGui::SetWindowPos(ImVec2(0, GetScreenHeight() - 25), 0);
			ImGui::TextColored(ImVec4(255, 255, 255, 255), ENGINE_VERSION);
			ImGui::End();
		}

		for each (GameObject ^ obj in scene->GetRenderQueue())
		{
			if (obj != nullptr)
			{
				if (obj != nullptr)
				{
					obj->GameDrawImGUI();
				}
			}
		}

		windowHandle->DrawImGui();

		rlImGuiEnd();
	}
	EndDrawing();

	PostRenderFrame();

	RLGL::rlReloadTextureUnits();
}

RAYLIB::RenderTexture* Engine::Render::ScriptableRenderPipeline::getFrameBuffer()
{
	return &framebufferTexturePtr->getInstance();
}

