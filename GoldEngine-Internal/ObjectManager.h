#pragma once

// System namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace Engine::Scripting
{
	public enum SceneTarget
	{
		RenderQueue,
		SceneObjects
	};

	/// <summary>
	/// Middle level class, allows for finding objects into the scene, and doing operations with them.
	/// Getting parents, etc.
	/// </summary>
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ObjectManager
	{
		// singleton
	private:
		static ObjectManager^ self;

	public:
		static ObjectManager^ singleton()
		{
			return self;
		}

	private:
		Engine::Management::Scene^ loadedScene;
		Engine::Scripting::SceneTarget target = Engine::Scripting::RenderQueue; // by default hook RenderQueue

	public:
		ObjectManager(Engine::Management::Scene^ loadedScene);

	private:
		Engine::Internal::Components::GameObject^ GetObjectFromScene(Engine::Management::MiddleLevel::SceneObject^ sceneObject)
		{
			return sceneObject->SerializeOutput();
		}

	private:
		bool topReferenceIsDatamodel(Engine::Internal::Components::GameObject^ object, String^ dataModelName)
		{
			if (object->transform->parent != nullptr)
				return topReferenceIsDatamodel(object->transform->parent->GetObject<Engine::Internal::Components::GameObject^>(), dataModelName);
			else
			{
				if (object->name == dataModelName)
					return true;
				else
					return false;
			}
		}

	public:

	#ifndef PRODUCTION_BUILD
		GameObject^ GetSelectedObject()
		{
			return (GameObject^)Singleton<Window^>::Instance->GetSelectedObject();
		}
	#endif

		Engine::Management::Scene^ GetLoadedScene()
		{
			return loadedScene;
		}

		GameObject^ GetDatamodel(String^ dataModelName);

		GameObject^ GetDatamodel(String^ dataModelName, bool createDataModel);

		List<Engine::Internal::Components::GameObject^>^ GetObjectsFromDatamodel(System::String^ datamodel);

		List<Engine::Internal::Components::GameObject^>^ GetObjectsByTag(System::String^ tag)
		{
			auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t->GetTag() == tag)
				{
					objects->Add(t);
				}
			}

			return objects;
		}

		List<Engine::Internal::Components::GameObject^>^ GetObjectsByName(System::String^ name)
		{
			auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

			if (loadedScene->GetRenderQueue() == nullptr)
				return nullptr;

			for each (GameObject ^ t in loadedScene->GetRenderQueue())
			{
				if (t->name == name)
				{
					objects->Add(t);
				}
			}

			return objects;
		}

		List<Engine::Internal::Components::GameObject^>^ GetObjects()
		{
			auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

			for each (GameObject ^ t in loadedScene->GetRenderQueue())
			{
				if(t != nullptr && t != nullptr)
					objects->Add(t);
			}

			return objects;
		}

		List<Engine::Internal::Components::GameObject^>^ GetObjectsOfType(Engine::Internal::Components::ObjectType type)
		{
			auto objects = gcnew List<Engine::Internal::Components::GameObject^>();

			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t != nullptr && t->GetObjectType() == type)
				{
					objects->Add(t);
				}
			}

			return objects;
		}

		generic <class T>
		List<T>^ GetObjectsOfType()
		{
			auto objects = gcnew List<T>();

			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t != nullptr && t->GetType() == T::typeid)
				{
					objects->Add(t->ToObjectType<T>());
				}
			}

			return objects;
		}

		Engine::Internal::Components::GameObject^ GetFirstObjectByTag(System::String^ tag)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t->GetTag() == tag)
				{
					return t;
				}
			}

			return nullptr;
		}

		Engine::Internal::Components::GameObject^ GetFirstObjectByName(System::String^ name)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t->name == name)
				{
					return t;
				}
			}

			return nullptr;
		}

		Engine::Internal::Components::GameObject^ GetFirstObjectOfType(System::String^ assemblyType)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t->GetType()->IsSubclassOf(Engine::EngineObjects::ScriptBehaviour::typeid) && t->ToObjectType<Engine::EngineObjects::ScriptBehaviour^>()->assemblyReference == assemblyType)
				{
					return t;
				}
			}
		}

		generic <class T>
		T GetFirstObjectOfType()
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t == nullptr)
					continue;

				if (t->GetType()->Equals(T::typeid) || t->GetType()->IsSubclassOf(T::typeid))
				{
					return (T)t;
				}
			}
		}

		Engine::Internal::Components::GameObject^ GetFirstObjectOfType(System::Type^ type)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t == nullptr)
					continue;

				if (t->GetType()->Equals(type) || t->GetType()->IsSubclassOf(type))
				{
					return t;
				}
			}
		}

		Engine::Internal::Components::GameObject^ GetFirstObjectOfType(Engine::Internal::Components::ObjectType type)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t == nullptr)
					continue;

				if (t->GetObjectType() == type)
				{
					return t;
				}
			}
		}

		List<Engine::Internal::Components::GameObject^>^ GetChildrenOf(Engine::Internal::Components::GameObject^ parent)
		{
			List<Engine::Internal::Components::GameObject^>^ newList = gcnew List<Engine::Internal::Components::GameObject^>();

			if (parent == nullptr)
				return newList;

			for each (GameObject ^ t in loadedScene->GetRenderQueue())
			{
				auto v = t;

				if (v->getTransform() == nullptr)
					continue;

				if (v->getTransform()->parent != nullptr)
				{
					if (v->getTransform()->GetParent()->GetUID() == parent->getTransform()->GetUID())
					{
						newList->Add(v);
					}
				}
			}

			return newList;
		}

		Engine::Internal::Components::GameObject^ GetObjectByUid(System::String^ uid)
		{
			for each (GameObject^ t in loadedScene->GetRenderQueue())
			{
				if (t->getTransform()->GetUID() == uid)
				{
					return t;
				}
			}

			return nullptr;
		}

		Engine::Internal::Components::GameObject^ GetObjectFromTransform(Engine::Internal::Components::Transform^ transform)
		{
			return GetObjectByUid(transform->GetUID());
		}

		Engine::EngineObjects::Camera^ GetMainCamera();
		Engine::EngineObjects::Camera^ GetMainCamera(bool ignoreEditorCameras);

		void Instantiate(Engine::Internal::Components::GameObject^ newObject)
		{
			loadedScene->AddObjectToScene(newObject);
		}

		void Destroy(Engine::Internal::Components::GameObject^ object);
		
		concurrency::task<bool> WaitForDatamodel(String^ datamodelName);
		System::Threading::Tasks::Task<bool>^ WaitForDatamodelAsync(String^ datamodelName);
		
		// patching attributes


	public:
		void PatchAttributeObject(Attribute^ attribute)
		{
			if (attribute == nullptr)
				return;

			if (attribute->getValue() != nullptr)
			{
				if ((attribute->getValue()->GetType()->Equals(Engine::Internal::Components::GameObject::typeid) || attribute->getValue()->GetType()->IsSubclassOf(Engine::Internal::Components::GameObject::typeid)))
				{
					String^ uid = ((Engine::Internal::Components::GameObject^)attribute->getValue())->getTransform()->GetUID();

					attribute->setValueForce(GetObjectByUid(uid), false);
				}
			}
			else
			{
				attribute->setValueForce(nullptr, false);
			}
		}

	};
}