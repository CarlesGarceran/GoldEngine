#pragma once
#include "EngineIncludes.h"

#ifdef USE_ILLUMINA

using namespace System;

namespace Engine::EngineObjects::Daemons
{
	public ref class LightDaemon : Engine::EngineObjects::Daemon
	{
	private:
		LightManager^ lightM;
		[JSON_SERIALIZE]
			unsigned int shaderId;

	public:
		LightDaemon(System::String^ name, Engine::Internal::Components::Transform^ transform, LightManager^ lightManager) : EngineObjects::Daemon(name, transform)
		{
			lightM = lightManager;
		}

		void Draw() override
		{
			if (lightM == nullptr)
			{
				lightM = Singleton<LightManager^>::Instance;
				printf("Lightmanager not set to an instance. Daemon won't work!");
				return;
			}

			Engine::EngineObjects::Camera^ camera = Singleton<ObjectManager^>::Instance->GetMainCamera();

			if (camera == nullptr)
				return;

			float cameraPos[3] = { camera->getTransform()->position.x, camera->getTransform()->position.y, camera->getTransform()->position.z };
			lightM->UpdateCameraPosition(cameraPos);
		}

		[ExecuteInEditModeAttribute]
			void Update() override
		{
			if (lightM == nullptr)
			{
				lightM = Singleton<LightManager^>::Instance;
				printf("Lightmanager not set to an instance. Daemon won't work!");
				return;
			}

			auto lights = Engine::Scripting::ObjectManager::singleton()->GetObjectsOfType(Engine::Internal::Components::ObjectType::LightSource);

			for each (auto light in lights)
			{
				if (light != nullptr)
				{
					Engine::EngineObjects::LightSource^ src = nullptr;
					try
					{
						src = (Engine::EngineObjects::LightSource^)light;

						if (src != nullptr)
						{
							if (!lightM->hasLight(src))
							{
								lightM->AddLight(src);
							}
						}
					}
					catch (Exception^ ex)
					{
						printError("Failed to cast <unknown-type> to GC_LightSource");
						throw ex;
					}
				}
			}

			lightM->LightUpdate();
		}
	};
}

#endif