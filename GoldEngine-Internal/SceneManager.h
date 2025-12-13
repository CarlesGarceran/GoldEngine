
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

		static void LoadSceneFromMemory(System::String^ sceneMetadata)
		{
			Engine::Management::Scene^ loadedScene = Engine::Management::Scene::getLoadedScene();
			unsigned int passwd = Engine::Encryption::CypherLib::GetPasswordBytes(Engine::Config::EngineSecrets::singleton()->encryptionPassword);

			LoadSceneFromMemory(sceneMetadata, passwd, loadedScene);
		}

		static void LoadSceneFromFile(System::String^ sceneName)
		{
			Engine::Management::Scene^ loadedScene = Engine::Management::Scene::getLoadedScene();
			unsigned int passwd = Engine::Encryption::CypherLib::GetPasswordBytes(Engine::Config::EngineSecrets::singleton()->encryptionPassword);

			LoadSceneFromFile(sceneName, passwd, loadedScene);
		}

		static void LoadSceneFromMemory(String^ sceneMetadata, unsigned int passwd, Engine::Management::Scene^% loadedScene)
		{
			auto parsedScene = Newtonsoft::Json::JsonConvert::DeserializeObject<Engine::Management::Scene^>(sceneMetadata);

			loadedScene->setPassword(passwd);
			// bridge all the unsetted values from the parsed scene
			loadedScene->assetPacks = parsedScene->assetPacks;
			loadedScene->sceneName = parsedScene->sceneName;
			loadedScene->skyColor = parsedScene->skyColor;
			loadedScene->sceneRequirements = parsedScene->sceneRequirements;


			cli::array<Engine::Management::MiddleLevel::SceneObject^>^ sceneObjects = parsedScene->GetDrawQueue();
			cli::array<GameObject^>^ persistentObjects = loadedScene->GetPersistentObjects();

			msclr::lock lock(sceneObjects);
			if (lock.try_acquire(1000))
			{
				auto data = sceneObjects;
				loadedScene->cleanupSceneObjects();
				parsedScene->cleanupSceneObjects();

				for each(auto object in data)
				{
					object->deserialize();
					parsedScene->AddObjectToScene(object->GetReference());
				}

				for each (auto obj in persistentObjects)
				{
					parsedScene->AddObjectToScene(obj);
				}
			}

			for each(GameObject ^ object in parsedScene->GetRenderQueue())
			{
				loadedScene->AddObjectToScene((GameObject^)object);
			}

			Engine::Management::Scene::getLoadedScene()->flagSceneLoaded(true);
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
				loadedScene->skyColor = parsedScene->skyColor;
				loadedScene->sceneRequirements = parsedScene->sceneRequirements;

				cli::array<Engine::Management::MiddleLevel::SceneObject^>^ sceneObjects = parsedScene->GetDrawQueue();
				cli::array<GameObject^>^ persistentObjects = loadedScene->GetPersistentObjects();

				msclr::lock lock(sceneObjects);
				if (lock.try_acquire(1000))
				{
					auto data = sceneObjects;
					loadedScene->cleanupSceneObjects();
					parsedScene->cleanupSceneObjects();

					for each (auto object in data)
					{
						object->deserialize();
						parsedScene->AddObjectToScene(object->GetReference());
					}

					for each (auto obj in persistentObjects)
					{
						parsedScene->AddObjectToScene(obj);
					}
				}

				for each (GameObject ^ object in parsedScene->GetRenderQueue())
				{
					loadedScene->AddObjectToScene((GameObject^)object);
				}

				Engine::Management::Scene::getLoadedScene()->flagSceneLoaded(true);
			}
			else
			{
				auto assetPacks = gcnew System::Collections::Generic::List<String^>();
				//assetPacks->Add("Data/engineassets.gold");
				loadedScene = gcnew Engine::Management::Scene(fN, "Assets_" + fN, assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF);
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
				return gcnew Engine::Management::Scene("Level0", "Assets_Level0", assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF);
			else
				return gcnew Engine::Management::Scene(sceneName, "Assets_" + sceneName, assetPacks, gcnew System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>(), 0x000000FF);
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