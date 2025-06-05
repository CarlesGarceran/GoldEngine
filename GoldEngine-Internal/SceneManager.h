
#pragma once

#include "EngineIncludes.h"
#include "ObjectManager.h"
#include "AsmLoader.h"
#include "Scene.h"

namespace Engine::Managers
{
	public ref class SceneManager
	{
	private:
		static System::Collections::Generic::List<EngineAssembly^>^ assemblyManager;

	public:
		static void SetAssemblyManager(System::Collections::Generic::List <EngineAssembly^>^ manager)
		{
			assemblyManager = manager;
		}

		static System::Collections::Generic::List<EngineAssembly^>^ GetAssemblyManager()
		{
			return assemblyManager;
		}

	public:
		static bool AssetExists(System::String^ fN)
		{
			return System::IO::File::Exists("Data/" + fN + ".scn");
		}

		static void LoadSceneFromFile(System::String^ sceneName)
		{
			Engine::Management::Scene^ loadedScene = Engine::Management::Scene::getLoadedScene();
			unsigned int passwd = CypherLib::GetPasswordBytes(Engine::Config::EngineSecrets::singleton()->encryptionPassword);

			LoadSceneFromFile(sceneName, passwd, loadedScene);
		}

		static void LoadSceneFromFile(System::String^ fN, unsigned int passwd, Engine::Management::Scene^% loadedScene)
		{
			if (AssetExists(fN))
			{
				auto fileContents = System::IO::File::ReadAllText("Data/" + fN + ".scn");
				auto parsedScene = Newtonsoft::Json::JsonConvert::DeserializeObject<Engine::Management::Scene^>(fileContents);

				if (loadedScene != nullptr)
				{
					loadedScene->UnloadScene();
					loadedScene = CreateScene(fN);
				}
				else
				{
					loadedScene = CreateScene(fN);
				}

				loadedScene->setPassword(passwd);
				// bridge all the unsetted values from the parsed scene
				loadedScene->assetPacks = parsedScene->assetPacks;
				loadedScene->sceneName = parsedScene->sceneName;
				loadedScene->scene_assemblies = parsedScene->scene_assemblies;
				loadedScene->skyColor = parsedScene->skyColor;
				loadedScene->sceneRequirements = parsedScene->sceneRequirements;


				List<Engine::Management::MiddleLevel::SceneObject^>^ sceneObjects = parsedScene->GetDrawQueue();

				msclr::lock lock(sceneObjects);
				if (lock.try_acquire(1000))
				{
					auto data = sceneObjects->ToArray();
					loadedScene->cleanupSceneObjects();
					parsedScene->cleanupSceneObjects();

					for each (auto object in data)
					{
						object->deserialize();
						parsedScene->AddObjectToScene(object->GetReference());
					}
				}

				for each (GameObject ^ object in parsedScene->GetRenderQueue())
				{
					loadedScene->AddObjectToScene((GameObject^)object);
				}

				/*
				for each (auto t in parsedScene->sceneObjects)
				{
					// TYPE DEF \\

					Engine::EngineObjects::Skybox^ instancedSkybox;
					Engine::EngineObjects::ModelRenderer^ instancedModelRenderer;

					// PARAMS \\

					auto referenceObject = (Engine::Internal::Components::GameObject^)t->GetReference();
					auto objectType = t->objectType;
					auto deserializedData = t->deserializedData;
					auto renderQueue = loadedScene->GetRenderQueue();

					switch (objectType)
					{
					case Engine::Internal::Components::ObjectType::Datamodel:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(
							sceneObject
						);
					}
					break;
					case Engine::Internal::Components::ObjectType::ModelRenderer:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(
							sceneObject
						);

						instancedModelRenderer = sceneObject->GetValue< Engine::EngineObjects::ModelRenderer^ >();
						Model model = DataPacks::singleton().GetModel(instancedModelRenderer->model);
						Shader shader = DataPacks::singleton().GetShader(instancedModelRenderer->shader);
						Texture texture = DataPacks::singleton().GetTexture2D(instancedModelRenderer->texture);
						unsigned int hex = instancedModelRenderer->tint;

						//instancedModelRenderer->SetNativeRenderer(new Engine::EngineObjects::Native::NativeModelRenderer(model, material, texture, hex));
					}
					break;
					case Engine::Internal::Components::ObjectType::Skybox:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(
							sceneObject
						);

						instancedSkybox = sceneObject->GetValue<Engine::EngineObjects::Skybox^>();
						instancedSkybox->Init(instancedSkybox->model, instancedSkybox->material, instancedSkybox->texture);
					}
					break;
					case Engine::Internal::Components::ObjectType::Script:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						Engine::EngineObjects::ScriptBehaviour^ script = (Engine::EngineObjects::ScriptBehaviour^)sceneObject->GetReference();

						for each (auto assembly in assemblyManager)
						{
							if (assembly != nullptr)
							{
								if (assembly->hasType(script->assemblyReference))
								{
									Engine::Scripting::Logging::Log(String::Format("Loading assembly type {0}", script->assemblyReference));

									auto obj = (Engine::Internal::Components::GameObject^)assembly->CastToType(sceneObject, script->assemblyReference);

									sceneObject->SetReference(obj);
								}
							}
						}
						script->Init();

						loadedScene->PushToRenderQueue(sceneObject);
					}
					break;

					case Engine::Internal::Components::ObjectType::CubeRenderer:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(
							sceneObject
						);

						auto cubeRenderer = sceneObject->GetValue<Engine::EngineObjects::CubeRenderer^>();
						cubeRenderer->Init(cubeRenderer->color);
					}
					break;

					case Engine::Internal::Components::ObjectType::PBR_ModelRenderer:
					{
						auto sceneObj = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(sceneObj);

						//pbrRenderer->Init(pbrRenderer->model_id, pbrRenderer->shader_id, pbrRenderer->texture_id, pbrRenderer->color_hex);
					}
					break;

					case Engine::Internal::Components::ObjectType::LightSource:
					{
						auto sceneObj = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(sceneObj);

						auto lightSource = sceneObj->GetValue<Engine::EngineObjects::LightSource^>();
						lightSource->Init(lightSource->lightColor, lightSource->intensity, lightSource->target, lightSource->lightType, lightSource->shaderId);
					}
					break;

					case Engine::Internal::Components::ObjectType::GridRenderer:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(sceneObject);

						auto gridRenderer = sceneObject->GetValue<Engine::EngineObjects::GridRenderer^>();
						gridRenderer->Init(gridRenderer->lines, gridRenderer->spacing);
					}
					break;

					case Engine::Internal::Components::ObjectType::BoundingBoxRenderer:
					{
						auto sceneObject = gcnew Engine::Management::MiddleLevel::SceneObject(
							objectType,
							nullptr,
							deserializedData
						);

						loadedScene->PushToRenderQueue(sceneObject);

						auto boundingBox = sceneObject->GetValue<Engine::EngineObjects::BoundingBoxRenderer^>();
						boundingBox->Init(boundingBox->color);
					}
					break;
					}
				}
				*/

				Engine::Management::Scene::getLoadedScene()->flagSceneLoaded(true);
			}
			else
			{
				auto assetPacks = gcnew System::Collections::Generic::List<String^>();
				//assetPacks->Add("Data/engineassets.gold");
				loadedScene = gcnew Engine::Management::Scene(fN, "Assets_" + fN, assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF, gcnew System::Collections::Generic::List<System::String^>());
			}

			if (loadedScene == nullptr)
				TraceLog(LOG_FATAL, "FAILED OPENING SCENE");

			loadedScene->Preload(assemblyManager);

			gcnew Engine::Scripting::ObjectManager(loadedScene);

			loadedScene->HookSceneInit();
		}

		static Engine::Management::Scene^ CreateScene(System::String^ sceneName)
		{
			auto assetPacks = gcnew System::Collections::Generic::List<String^>();
			//assetPacks->Add("Data/engineassets.gold");

			if (sceneName->Equals(""))
				return gcnew Engine::Management::Scene("Level0", "Assets_Level0", assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF, gcnew System::Collections::Generic::List<System::String^>());
			else
				return gcnew Engine::Management::Scene(sceneName, "Assets_" + sceneName, assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF, gcnew System::Collections::Generic::List<System::String^>());
		}

		static void SaveSceneToFile(Engine::Management::Scene^ scene, unsigned int password)
		{
			scene->SerializeObjects();

			if (scene->sceneName)
			{
				String^ serializedData = Newtonsoft::Json::JsonConvert::SerializeObject(scene, Newtonsoft::Json::Formatting::Indented);
				//String^ cipheredContents = CypherLib::EncryptFileContents(serializedData, password);

				//System::IO::File::WriteAllText("Data/" + scene->sceneName + ".scn", System::Convert::ToBase64String(Encoding::UTF8->GetBytes(cipheredContents)));
				System::IO::File::WriteAllText("Data/" + scene->sceneName + ".scn", serializedData);
			}
			else
			{
				// create file and rerun save
				System::IO::File::Create("Data/" + scene->sceneName + ".scn")->Close();
				SaveSceneToFile(scene, password);
			}
		}

		static void UnloadScene(Engine::Management::Scene^ loadedScene)
		{
			loadedScene->UnloadScene();
		}

	};
}