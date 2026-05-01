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

		static System::Collections::Generic::List<Engine::Internal::Components::GameObject^>^ persistantObjects = gcnew System::Collections::Generic::List<Engine::Internal::Components::GameObject^>();

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

			if (Directory::Exists(EXTRACT_PATH))
				Directory::Delete(EXTRACT_PATH, true);

			OnLoad();

		}

		void UnloadScene()
		{
			singleton = nullptr;
			sceneFinishedLoading = false;

			printConsole("Unloading scene");
			OnUnload();
			derreferenceSceneObjects();
			sceneObjects->Clear();
			RLGL::rlReloadTextureUnits();
			System::GC::Collect();
			System::GC::WaitForPendingFinalizers();
		}

		void RemoveObjectFromScene(Engine::Management::MiddleLevel::SceneObject^ object)
		{
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

		cli::array<Engine::Management::MiddleLevel::SceneObject^>^ GetDrawQueue()
		{
			return sceneObjects->ToArray();
		}

		cli::array<Engine::Internal::Components::GameObject^>^ GetPersistentObjects()
		{
			return persistantObjects->ToArray();
		}

	internal:
		void cleanupSceneObjects()
		{
			sceneObjects->Clear();
		}

		void derreferenceSceneObjects()
		{
			for (int x = 0; x < sceneObjects->Count; x++)
			{
				auto sceneObject = sceneObjects[x];

				if (sceneObject != nullptr)
				{
					if (persistantObjects->Contains(sceneObject->GetReference())) continue;

					delete sceneObject;
					sceneObject = nullptr;
				}
			}
		}

		Engine::Management::MiddleLevel::SceneObject^ getSceneObject(Engine::Internal::Components::GameObject^ object)
		{
			Engine::Management::MiddleLevel::SceneObject^ instance = nullptr;

			for each(Engine::Management::MiddleLevel::SceneObject ^ sceneObject in sceneObjects)
			{
				GameObject^ inst = sceneObject->GetReference();

				if (inst == object)
				{
					instance = sceneObject;
					break;
				}
			}

			return instance;
		}

		void derreferenceObject(Engine::Management::MiddleLevel::SceneObject^ object)
		{
			if (object == nullptr) return;

			delete object;
			object = nullptr;
		}

	public:
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
			else
			{
				return nullptr;
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

			msclr::lock l(sceneObjects);
			sceneObjects->Add(gcnew Engine::Management::MiddleLevel::SceneObject(
				object->GetObjectType(),
				object,
				""));

			try
			{
				object->InitializeObject();

				if (sceneFinishedLoading)
				{
					object->Setup();
					object->Init();
					object->Awake();
					object->Start();
				}
			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
				printError(ex->StackTrace);
			}
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
					Engine::Components::Quaternion::FromEulerAngles(Engine::Components::Vector3(0, 0, 0)),
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
			auto renderQueue = this->GetRenderQueue();

			// 1️⃣ Setup phase
			for each (GameObject ^ obj in renderQueue)
			{
				if (obj == nullptr) continue;

				try
				{
					obj->Setup();
				}
				catch (Exception^ ex)
				{
					printError(ex->Message);
					printError(ex->StackTrace);
				}
			}

			// 2️⃣ Init phase
			for each (GameObject ^ obj in renderQueue)
			{
				if (obj == nullptr) continue;

				try
				{
					obj->Init();
				}
				catch (Exception^ ex)
				{
					printError(ex->Message);
					printError(ex->StackTrace);
				}
			}

			// 3️⃣ Awake phase
			for each (GameObject ^ obj in renderQueue)
			{
				if (obj == nullptr) continue;

				try
				{
					obj->Awake();
				}
				catch (Exception^ ex)
				{
					printError(ex->Message);
					printError(ex->StackTrace);
				}
			}

			// 4️⃣ Start phase
			for each (GameObject ^ obj in renderQueue)
			{
				if (obj == nullptr) continue;

				try
				{
					obj->Start();
				}
				catch (Exception^ ex)
				{
					printError(ex->Message);
					printError(ex->StackTrace);
				}
			}
		}

		// VMethods
	public:
		virtual void OnUnload()
		{
			std::list<int> indicesToPurge = {};
			for (int x = 0; x < persistantObjects->Count; x++)
			{
				if (persistantObjects[x]->IsDisposed()) indicesToPurge.push_back(x);
			}

			indicesToPurge.reverse();
			for (int index : indicesToPurge)
			{
				persistantObjects->RemoveAt(index);
			}

			for each(auto object in sceneObjects->ToArray())
			{
				if (object == nullptr) continue;

				Engine::Internal::Components::GameObject^ ref;
				bool unload = true;
				if ((ref = object->GetReference()) != nullptr)
				{
					auto attribs = ref->GetType()->GetCustomAttributes(true);

					for each (auto attr in attribs)
					{
						if (attr->GetType() == Engine::Scripting::PersistantInstanceAttribute::typeid) 
						{
							if(!persistantObjects->Contains(ref)) persistantObjects->Add(ref);
							unload = false;
							continue;
						}
					}

					if (unload) 
					{
						Engine::Internal::Components::GameObject::Destroy(ref);
						delete object;
					}
				}
			}

			System::GC::Collect();
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