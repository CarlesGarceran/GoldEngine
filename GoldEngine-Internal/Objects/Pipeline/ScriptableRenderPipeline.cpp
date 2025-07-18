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

void Engine::Render::unloadCameraData(cameraData* data)
{
	delete data;
}

void Engine::Render::CallImGuizmoRender(Engine::Window^ windowPtr)
{
	((EditorWindow^)windowPtr)->DrawImGuizmo();
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

void Engine::Render::ScriptableRenderPipeline::ExecuteRenderWorkflow(Engine::Window^ windowHandle, Engine::Management::Scene^ scene)
{
	auto c = gcnew Engine::Components::Color(scene->skyColor);
	RAYLIB::Color clearColor = c->toNative();

	PreFirstPassRender(scene);
	PostFirstPassRender();

	BeginDrawing();
	{
		CreateTexture();

		BeginTextureMode(this->framebufferTexturePtr->getInstance());

		OnRenderBegin();

		ClearBackground(clearColor);
		RLGL::rlClearScreenBuffers();
		RLGL::rlEnableDepthTest();
		PreRenderFrame();
		{
			Engine::EngineObjects::Camera^ camera = Engine::Scripting::ObjectManager::singleton()->GetMainCamera(true);

			int currentLayer = 0;

			if (camera == nullptr)
				goto RENDER_END;

			bool is3DCamera = camera->is3DCamera();

			if (cameraDataPtr != nullptr)
				delete cameraDataPtr;

			cameraDataPtr = new Engine::Native::EnginePtr<cameraData*>(new cameraData(camera->nearPlane, camera->farPlane), &unloadCameraData);

			auto camera3D = ((Engine::EngineObjects::Native::NativeCamera3D*)camera->get());

			BeginMode3D(camera3D->get());

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

		PreRenderStack();

		ClearBackground(clearColor);

		RAYLIB::Rectangle target;
		target.x = 0;
		target.y = 0;
		target.width = Engine::Scripting::Screen::Width;
		target.height = -Engine::Scripting::Screen::Height;

		DrawTextureRec(framebufferTexturePtr->getInstance().texture, target, { 0,0 }, { 255,255,255,255 });

		PostRenderStack();

		OnRenderEnd();

		rlImGuiBegin();

		for each (Engine::Internal::Components::GameObject ^ obj in scene->GetRenderQueue())
		{
			if (obj != nullptr)
			{
				if (obj != nullptr)
				{
					obj->GameDrawImGUI();
				}
			}
		}

		//windowHandle->DrawImGui();

		rlImGuiEnd();
	}
	EndDrawing();
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

		RLGL::rlClearScreenBuffers();
		RLGL::rlEnableDepthTest();

		PreRenderFrame(); // PRE FRAME
		{
			Engine::EngineObjects::Camera^ camera = Engine::Scripting::ObjectManager::singleton()->GetMainCamera(false);

			int currentLayer = 1;

			if (camera == nullptr)
				goto RENDER_END;

			bool is3DCamera = camera->is3DCamera();

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
}

RAYLIB::RenderTexture* Engine::Render::ScriptableRenderPipeline::getFrameBuffer()
{
	return &framebufferTexturePtr->getInstance();
}

