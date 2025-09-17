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

	void CallImGuizmoRender(Engine::Window^ windowPtr);

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


		RAYLIB::RenderTexture* getFrameBuffer();
		RAYLIB::RenderTexture* GetFrameBuffer() { return getFrameBuffer(); }

	public:
		ScriptableRenderPipeline();


	private:
		bool isInRenderSurface(GameObject^ gameObject);

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

	public:
		RenderTexture2D LoadRenderTextureDepthTex(int width, int height);

	public:
		inline void ExecuteRenderWorkflow(Engine::Window^ windowHandle, Engine::Management::Scene^ scene);
		inline void ExecuteRenderWorkflow_Editor(Engine::Window^ windowHandle, Engine::Management::Scene^ scene);

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

							if (isInRenderSurface(reference))
								continue;

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


