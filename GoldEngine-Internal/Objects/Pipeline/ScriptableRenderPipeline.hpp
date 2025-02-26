#pragma once

namespace Engine::Render
{
	struct cameraData
	{
	public:
		float nearPlane;
		float farPlane;

		cameraData(float nP, float fP)
		{
			this->nearPlane = nP;
			this->farPlane = fP;
		}
	};

	void UnloadRenderTextureDepthTex(RenderTexture2D target);
	void unloadCameraData(cameraData* data);

	public ref class ScriptableEffect abstract
	{
	public:
		int width = 1920;
		int height = 1080;


	public:
		virtual void OnEffectBegin() abstract;
		virtual void OnEffectEnd() abstract;

		virtual void OnEffectApply(RAYLIB::RenderTexture2D* fboPtr) {};

		virtual void OnEffectUnload() abstract;

		virtual void SetFramebuffer(RAYLIB::RenderTexture2D* FrameBuffer) {};

		virtual void SetTexture(RAYLIB::Texture2D* texture) {};
		virtual void SetDepth(RAYLIB::Texture2D* texture) {};

		virtual bool ManualRendering() { return false; } // Manual rendering, if this option is triggered the rendering will be handled by the game engine instead of the Effect
	};

	public ref class ScriptableRenderPipeline abstract
	{
	protected:
		int height, width;

	public:
		Engine::Native::EnginePtr<RAYLIB::RenderTexture>* savedTexPtr; // used for the effects, interwinds with framebuffer.
		Engine::Native::EnginePtr<RAYLIB::RenderTexture>* framebufferTexturePtr;
		Engine::Native::EnginePtr<RAYLIB::Shader>* depthShaderPtr;
		Engine::Native::EnginePtr<cameraData*>* cameraDataPtr;

		System::Collections::Generic::List<ScriptableEffect^>^ effects;

	public:
		ScriptableRenderPipeline()
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

	protected:
		void CreateTexture()
		{
			if ((this->height != Engine::Scripting::Screen::Height) || (this->width != Engine::Scripting::Screen::Width))
			{
				if (framebufferTexturePtr != nullptr)
					delete framebufferTexturePtr;

				RAYLIB::RenderTexture renderTexture = LoadRenderTextureDepthTex(Engine::Scripting::Screen::Width, Engine::Scripting::Screen::Height);
				framebufferTexturePtr = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(renderTexture, &UnloadRenderTextureDepthTex);
				RAYLIB::RenderTexture rt2 = LoadRenderTextureDepthTex(Engine::Scripting::Screen::Width, Engine::Scripting::Screen::Height);
				savedTexPtr = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(rt2, &UnloadRenderTextureDepthTex);

				this->width = Engine::Scripting::Screen::Width;
				this->height = Engine::Scripting::Screen::Height;
			}
		}

		RenderTexture2D LoadRenderTextureDepthTex(int width, int height)
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
				target.depth.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;       //DEPTH_COMPONENT_24BIT?
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

	public:
		void ExecuteRenderWorkflow(Engine::Window^ windowHandle, Engine::Management::Scene^ scene)
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
							effect->OnEffectApply(&framebufferTexturePtr->getInstance());
						else
						{
							effect->OnEffectBegin();

							Rectangle target;
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

						Rectangle target;
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

				Rectangle target;
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

		void ExecuteRenderWorkflow_Editor(Engine::Window^ windowHandle, Engine::Management::Scene^ scene)
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
							effect->OnEffectApply(&framebufferTexturePtr->getInstance());
						else
						{
							effect->OnEffectBegin();

							Rectangle target;
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

						Rectangle target;
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

	protected:
		void renderGUI(int currentLayer, Engine::Management::Scene^ scene)
		{
			Layer^ lastLayer = nullptr;

			while (currentLayer != Engine::Scripting::LayerManager::getHigherLayer())
			{
				Layer^ cL = Engine::Scripting::LayerManager::GetLayerFromId(currentLayer);

				if (cL != nullptr)
				{
					for each (Engine::Internal::Components::GameObject ^ sceneObject in scene->GetRenderQueue())
					{
						if (scene->sceneLoaded())
						{
							Engine::Internal::Components::GameObject^ reference = (Engine::Internal::Components::GameObject^)sceneObject;

							if (reference->layerMask->IsLayer(cL))
							{
								if (reference->active)
									reference->DrawGUI();
							}
						}
					}

					Layer^ nextLayer = Engine::Scripting::LayerManager::getNextHigherLayer(cL);

					if (nextLayer != nullptr)
						currentLayer = nextLayer->layerMask;
					else
						break;

					lastLayer = cL;
				}
				else
				{
					if (lastLayer == nullptr)
						break;

					Layer^ nextLayer = Engine::Scripting::LayerManager::getNextHigherLayer(lastLayer);

					if (nextLayer != nullptr)
						currentLayer = nextLayer->layerMask;
					else
						break;
				}

			}
		}

	protected:
		void render(int currentLayer, Engine::Management::Scene^ scene)
		{
			if (currentLayer == Engine::Scripting::LayerManager::getHigherLayer())
				return;
			else
			{
				//ObjectManager::singleton()->GetFirstObjectOfType<Engine::EngineObjects::LightManager^>()->LightUpdate();

				PreRenderObjects();

				Layer^ cL = Engine::Scripting::LayerManager::GetLayerFromId(currentLayer);

				if (cL != nullptr)
				{
					if (scene->sceneLoaded())
					{
						for each (Engine::Internal::Components::GameObject ^ sceneObject in scene->GetRenderQueue())
						{
							Engine::Internal::Components::GameObject^ reference = (Engine::Internal::Components::GameObject^)sceneObject;

							try
							{
								if (reference->layerMask->IsLayer(cL))
								{
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_ALPHA))
										RAYLIB::BeginBlendMode(BLEND_ALPHA);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_ADDITIVE))
										RAYLIB::BeginBlendMode(BLEND_ADDITIVE);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_MULTIPLIED))
										RAYLIB::BeginBlendMode(BLEND_MULTIPLIED);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_ADD_COLORS))
										RAYLIB::BeginBlendMode(BLEND_ADD_COLORS);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_SUBTRACT_COLORS))
										RAYLIB::BeginBlendMode(BLEND_SUBTRACT_COLORS);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_ALPHA_PREMULTIPLY))
										RAYLIB::BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_CUSTOM))
										RAYLIB::BeginBlendMode(BLEND_CUSTOM);
									if ((reference->layerMask->getLayerBlendFlags() & BLEND_CUSTOM_SEPARATE))
										RAYLIB::BeginBlendMode(BLEND_CUSTOM_SEPARATE);

									PreRenderObject(reference);

									/*
									reference->GameDraw();
									*/
#if PRODUCTION_BUILD == FALSE

									if (!EngineState::PlayMode)
									{
										reference->GameDrawGizmos();
									}

#endif

									PostRenderObject();

									RAYLIB::EndBlendMode();
								}
							}
							catch (Exception^ ex)
							{
								print("[Drawing Managed Exception]", "An exception has occurred within the draw loop -> " + ex->Message);
							}
							catch (std::exception ex)
							{
								print("[Drawing Unmanaged Exception]", "An exception has occurred within the draw loop -> " + gcnew String(ex.what()));
							}
						}
					}
					Layer^ nextLayer = Engine::Scripting::LayerManager::getNextHigherLayer(cL);

					if (nextLayer != nullptr)
						return render(nextLayer->layerMask, scene);
					else
						return;
				}
				else
					return render(currentLayer++, scene);

				PostRenderObjects();
			}

		}

		void firstPassRender(int currentLayer) override
		{
			if (Engine::Scripting::ObjectManager::singleton() == nullptr || !Singleton<Engine::Management::Scene^>::Instantiated)
				return;

			Engine::Management::Scene^ scene = Singleton<Engine::Management::Scene^>::Instance;

			while (currentLayer != Engine::Scripting::LayerManager::getHigherLayer())
			{
				Layer^ cL = Engine::Scripting::LayerManager::GetLayerFromId(currentLayer);

				if (cL == nullptr)
					continue;

				for each (Engine::Internal::Components::GameObject ^ sceneObject in scene->GetRenderQueue())
				{
					if (scene->sceneLoaded())
					{
						Engine::Internal::Components::GameObject^ reference = (Engine::Internal::Components::GameObject^)sceneObject;

						if (reference->layerMask->IsLayer(cL))
						{
							reference->Draw();
							reference->DrawGizmo();
						}
					}
				}

				Layer^ nextLayer = Engine::Scripting::LayerManager::getNextHigherLayer(cL);

				if (nextLayer != nullptr)
					currentLayer = nextLayer->layerMask;
				else
					break;
			}
		}

		void firstPassRender(int currentLayer, System::Collections::Generic::List< Engine::Internal::Components::GameObject^>^% objects)
		{
			if (Engine::Scripting::ObjectManager::singleton() == nullptr || !Singleton<Engine::Management::Scene^>::Instantiated)
				return;

			Engine::Management::Scene^ scene = Singleton<Engine::Management::Scene^>::Instance;

			while (currentLayer != Engine::Scripting::LayerManager::getHigherLayer())
			{
				Layer^ cL = Engine::Scripting::LayerManager::GetLayerFromId(currentLayer);

				if (cL == nullptr)
					continue;

				for each (Engine::Internal::Components::GameObject ^ sceneObject in scene->GetRenderQueue())
				{
					if (scene->sceneLoaded())
					{
						Engine::Internal::Components::GameObject^ reference = (Engine::Internal::Components::GameObject^)sceneObject;

						if (reference->layerMask->IsLayer(cL))
						{
							objects->Add(reference);
							reference->Draw();
							reference->DrawGizmo();
						}
					}
				}

				Layer^ nextLayer = Engine::Scripting::LayerManager::getNextHigherLayer(cL);

				if (nextLayer != nullptr)
					currentLayer = nextLayer->layerMask;
				else
					break;
			}
		}

	public:
		// Render Per Frame
		virtual void PreRenderFrame() abstract;
		virtual void PostRenderFrame() abstract;

		// Rendering per object stack
		virtual void PreRenderObjects() abstract;
		virtual void PostRenderObjects() abstract;

		// Render Per Object
		virtual void PreRenderObject(Engine::Internal::Components::GameObject^) abstract;
		virtual void PostRenderObject() abstract;

		virtual void PreRenderStack() abstract;
		virtual void PostRenderStack() abstract;

		// Hooks at the beggining and end of the render process
		virtual void OnRenderBegin() abstract;
		virtual void OnRenderEnd() abstract;

		// FirstPass Rendering
		virtual void PreFirstPassRender(Engine::Management::Scene^) abstract;
		virtual void PostFirstPassRender() abstract;

		// For freeing resources and memory
		virtual void OnUnloadPipeline()
		{
			delete framebufferTexturePtr;
			delete depthShaderPtr;

			for each (ScriptableEffect ^ effect in effects)
			{
				effect->OnEffectUnload();
			}
		};
	};
}


