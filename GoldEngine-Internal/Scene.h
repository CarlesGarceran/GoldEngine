#pragma once
#include "DataPacks.h"
#include "DataPack.h"
#include "FileManager.h"
#include "SceneObject.h"
#include "AsmLoader.h"
#include "PreloadScript.h"

using namespace Engine::Assets::IO;
using namespace Engine::Assets::Storage;
using namespace Engine::Assets::Management;

namespace Engine::Management
{
	public delegate void OnSceneLoaded();

	public ref class Scene
	{
		// Privates
	private:
		System::Collections::Generic::List<EngineAssembly^>^ assemblies; // loaded assemblies -> passcall from scene_assemblies -> get assemblies for preloading -> tbh idk what more.
		DataPack^ sceneDatapack;
		bool sceneFinishedLoading;
		unsigned int password;
		static Scene^ singleton;
		event OnSceneLoaded^ onLoadedScene;
		event OnSceneLoaded^ raiseSetup;
		event OnSceneLoaded^ raiseInit;
		event OnSceneLoaded^ raiseAwake;

		// Properties
	public:
		System::String^ sceneName;
		System::Collections::Generic::List<System::String^>^ assetPacks;
		System::String^ sceneRequirements;
		unsigned long skyColor;
		System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>^ sceneObjects; // Managed storage + object parsing.

		// Constructors
	public:
		Scene(String^ name, String^ sR, System::Collections::Generic::List<String^>^ assetP, System::Collections::Generic::List<Engine::Management::MiddleLevel::SceneObject^>^ sceneO, unsigned long skyTint)
		{
			Singleton<Scene^>::Create(this);

			singleton = this;

			this->sceneName = name;
			this->assetPacks = assetP;
			this->sceneRequirements = sR;
			this->sceneObjects = sceneO;
			this->skyColor = skyTint;
			this->sceneDatapack = gcnew DataPack(sceneRequirements);
			this->sceneFinishedLoading = false;
		}

	public:
		void setPassword(unsigned int passwd)
		{
			password = passwd;
		}

		// Methods
	public:
		DataPack^ getSceneDataPack()
		{
			return sceneDatapack;
		}

		bool sceneLoaded() { return sceneFinishedLoading; }

		void flagSceneLoaded(bool value) { sceneFinishedLoading = value; }

		static Scene^ getLoadedScene()
		{
			return singleton;
		}

		void LoadScene()
		{
			printConsole("Loading scene " + sceneName);

			if (DataPacks::singleton().dataPackHasAssets())
				DataPacks::singleton().FreeAll(); // free all the assets

			DataPacks::singleton().LoadDefaultAssets();

			for each (auto packRoute in assetPacks)
			{
				printConsole("Loading asset -> " + packRoute);

				Engine::Assets::IO::FileManager::ReadCustomFileFormat(packRoute, Engine::Config::EngineSecrets::singleton()->encryptionPassword);
			}

			sceneDatapack->setFile(sceneRequirements);
			sceneDatapack->ReadFromFile(sceneRequirements, password);

			printConsole("All AssetsPacks have been unpacked and loaded into memory");

			sceneFinishedLoading = true;

			if (Directory::Exists("Data/unpacked/"))
				Directory::Delete("Data/unpacked/", true);

			OnLoad();

		}

		void UnloadScene()
		{
			sceneFinishedLoading = false;

			printConsole("Unloading scene");

			OnUnload();
			sceneObjects->Clear();
			DataPacks::singleton().FreeAll();
			RLGL::rlReloadTextureUnits();
			System::GC::Collect();
		}

		void RemoveObjectFromScene(Engine::Management::MiddleLevel::SceneObject^ object)
		{
			if (sceneObjects->Contains(object))
			{
				sceneObjects->Remove(object);
			}

			if (sceneObjects->Contains(object))
			{
				sceneObjects->Remove(object);
			}
		}

		void RemoveObjectFromScene(GameObject^ object)
		{
			Engine::Management::MiddleLevel::SceneObject^ instance;

			for each (Engine::Management::MiddleLevel::SceneObject ^ sceneObject in sceneObjects)
			{
				GameObject^ inst = sceneObject->GetReference();

				if (inst == object)
				{
					instance = sceneObject;
					break;
				}
			}

			if (sceneObjects->Contains(instance))
			{
				sceneObjects->Remove(instance);
			}
		}

		void AddObjectToScene(Engine::Internal::Components::GameObject^ object)
		{
			PushToRenderQueue(object);
		}

		List<GameObject^>^ GetRenderQueue()
		{
			List<GameObject^>^ Data = gcnew List<GameObject^>();

			for each (Engine::Management::MiddleLevel::SceneObject ^ object in sceneObjects)
			{
				if (object != nullptr && object->GetReference() != nullptr)
					Data->Add(object->GetReference());
			}

			return Data;
		}

		List<Engine::Management::MiddleLevel::SceneObject^>^ GetDrawQueue()
		{
			return sceneObjects;
		}

		void cleanupSceneObjects()
		{
			sceneObjects->Clear();
		}

		bool ExistsDatamodelMember(System::String^ datamodel)
		{
			for each (auto objects in sceneObjects)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = (Engine::Management::MiddleLevel::SceneObject^)objects;

				if (sceneObject->objectType == Engine::Internal::Components::ObjectType::Datamodel)
				{
					if (sceneObject->GetReference()->name == datamodel)
						return true;
				}
			}

			return false;
		}

		bool ExistsMember(System::String^ datamodel)
		{
			for each (auto objects in sceneObjects)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = (Engine::Management::MiddleLevel::SceneObject^)objects;

				if (sceneObject->GetReference()->name == datamodel)
					return true;
			}

			return false;
		}

		Engine::Internal::Components::GameObject^ GetMember(System::String^ datamodel)
		{
			Engine::Internal::Components::GameObject^ retn = nullptr;
			for each (auto objects in sceneObjects)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = (Engine::Management::MiddleLevel::SceneObject^)objects;

				if (sceneObject->GetReference()->name == datamodel)
					return sceneObject->GetReference();
			}

			return nullptr;
		}

		Engine::Internal::Components::GameObject^ GetDatamodelMember(System::String^ datamodel)
		{
			Engine::Internal::Components::GameObject^ retn = nullptr;
			for each (auto objects in sceneObjects)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = (Engine::Management::MiddleLevel::SceneObject^)objects;

				if (sceneObject->objectType == Engine::Internal::Components::ObjectType::Datamodel)
				{
					if (sceneObject->GetReference()->name == datamodel)
						return sceneObject->GetReference();
				}
			}

			return retn;
		}


		Engine::Internal::Components::GameObject^ GetDatamodelMember(System::String^ datamodel, bool create)
		{
			for each (auto objects in sceneObjects)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = (Engine::Management::MiddleLevel::SceneObject^)objects;

				if (sceneObject->objectType == Engine::Internal::Components::ObjectType::Datamodel)
				{
					if (sceneObject->GetReference()->name == datamodel)
						return sceneObject->GetReference();
				}
			}

			if (create)
			{
				auto newMember = AddDatamodelMember(datamodel);
				auto newObject = gcnew Engine::Management::MiddleLevel::SceneObject(Engine::Internal::Components::ObjectType::Datamodel, newMember, "");

				AddObjectToScene(newMember);

				return newMember;
			}
		}

		void CopyRenderQueueToSceneObjects()
		{
			for (int x = 0; x < sceneObjects->Count; x++)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = sceneObjects[x];

				sceneObject->deserialize();
			}
		}

		void SerializeObjects()
		{
			for (int x = 0; x < sceneObjects->Count; x++)
			{
				Engine::Management::MiddleLevel::SceneObject^ sceneObject = sceneObjects[x];

				sceneObject->serialize();
			}
		}

		void PushToRenderQueue(Engine::Internal::Components::GameObject^ object)
		{
			if (object == nullptr)
				return;

			msclr::lock^ l = gcnew msclr::lock(sceneObjects);
			l->acquire();
			{
				sceneObjects->Add(gcnew Engine::Management::MiddleLevel::SceneObject(
					object->GetObjectType(),
					object,
					""));

				if (!sceneFinishedLoading)
				{
					raiseSetup += gcnew OnSceneLoaded(object, &Engine::Internal::Components::GameObject::Setup);
					raiseInit += gcnew OnSceneLoaded(object, &Engine::Internal::Components::GameObject::Init);
					raiseAwake += gcnew OnSceneLoaded(object, &Engine::Internal::Components::GameObject::Awake);
					onLoadedScene += gcnew OnSceneLoaded(object, &Engine::Internal::Components::GameObject::Start);
				}
				else
				{
					try
					{
						object->Setup();
						object->Init();
						object->Awake();
						object->Start();
					}
					catch (Exception^ ex)
					{
						printError(ex->Message);
						printError(ex->StackTrace);
					}
				}
			}
			l->release();
		}

		void PushToRenderQueue(Engine::Management::MiddleLevel::SceneObject^ object)
		{
			PushToRenderQueue(object->GetReference());
		}

		Engine::Internal::Components::GameObject^ GetObjectByNameFromDrawQueue(System::String^ name)
		{
			Engine::Internal::Components::GameObject^ object = nullptr;

			for each (GameObject^ obj in GetRenderQueue())
			{
				if (obj->name == name)
				{
					object = obj;
					break;
				}
			}

			return object;
		}

	private protected:
		Engine::Internal::Components::GameObject^ AddDatamodelMember(System::String^ datamodel)
		{
			return gcnew Engine::Internal::Components::GameObject(datamodel,
				gcnew Engine::Internal::Components::Transform(
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(1, 1, 1),
					nullptr
				),
				Engine::Internal::Components::ObjectType::Datamodel,
				"",
				Engine::Scripting::LayerManager::GetLayerFromId(0)
			);
		}

	public:
		void HookSceneInit()
		{
			try
			{
				raiseSetup();
				raiseInit();
				raiseAwake();
				onLoadedScene();
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
				printError(ex->StackTrace);
			}
		}

		// VMethods
	public:
		virtual void OnUnload()
		{

		}
		virtual void OnLoad()
		{

		}

		virtual void Draw()
		{

		}
		virtual void Update()
		{

		}
		virtual void PhysicsUpdate()
		{

		}
		virtual void Preload(List<EngineAssembly^>^ asms)
		{
			assemblies = asms;

			for each (EngineAssembly ^ assm in assemblies)
			{
				for each (Type ^ asmType in assm->getPreloadScripts())
				{
					try
					{
						if (((String^)asmType->GetMethod("GetTarget")->Invoke(nullptr, nullptr))->Equals(this->sceneName) || asmType->GetMethod("GetTarget")->Invoke(nullptr, nullptr)->Equals("*"))
						{
							asmType->GetMethod("Preload")->Invoke(nullptr, nullptr);
						}
					}
					catch (Exception^ ex)
					{
						printError(ex->Message);
						printError(ex->StackTrace);
					}
				}
			}

			LoadScene();
		}
	};
}