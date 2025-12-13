#include "../Includes.h"
#include "../ManagedIncludes.h"
#include "../GlIncludes.h"
#include "../CastToNative.h"
#include "ObjectType.h"
#include "ViewSpace.h"
#include "../Cast.h"
#include "Layer.h"
#include "LayerManager.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "../LoggingAPI.h"
#include "../EngineState.h"
#include "../Event.h"
#include "Transform.h"
#include "../Reflection/ReflectedType.h"
#include "GameObject.h"
#include "../SDK.h"
#include "../Objects/Physics/CollisionType.h"
#include "../Objects/Physics/Native/NativePhysicsService.h"
#include "../Objects/Physics/RigidBody.h"
#include "../Objects/Physics/PhysicsService.h"
#include "../native/CollisionShape.h"
#include "../ObjectManager.h"

using namespace Engine::Internal::Components;

bool activeToggle = false;

#ifdef USE_BULLET_PHYS

Engine::Native::CollisionShape* getCollider(GameObject^ inst)
{
	Engine::Native::CollisionShape* ptr = (Engine::Native::CollisionShape*)inst->getCollisionShape();

	if (ptr == nullptr)
		inst->createCollisionShape();

	return (Engine::Native::CollisionShape*)inst->getCollisionShape();
}

void GameObject::createCollisionShape()
{
	this->collisionShape = new Engine::Native::CollisionShape(this);
}

#endif

void MoveChildren(GameObject^ root, cli::array<GameObject^>^ childs)
{
	for each (GameObject ^ child in childs)
	{
		//child->transform->position = (root->transform->position - child->transform->localPosition);
	}
}

// BUBBLING \\

void GameObject::descendantAdded(GameObject^ descendant)
{
	if (this->transform->parent != nullptr)
	{
		String^ TUID = transform->GetParent()->GetUID();

		Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(TUID)->onDescendantAdded->raiseExecution(gcnew cli::array<GameObject^> { descendant });
	}
}

// CTOR \\

GameObject::GameObject()
{
	this->coroutines = gcnew List<System::Collections::IEnumerator^>();

#ifdef USE_BULLET_PHYS
	this->collisionShape = new Engine::Native::CollisionShape(this);
#endif

	if(layerMask != nullptr)
		this->layerMask = Engine::Scripting::LayerManager::GetLayerFromId(layerMask->layerMask);

	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();
	this->onChildAdded = gcnew Engine::Scripting::Events::Event();
	this->onChildRemoved = gcnew Engine::Scripting::Events::Event();
	this->onDescendantAdded = gcnew Engine::Scripting::Events::Event();

	this->onDescendantAdded->connect(gcnew Action<GameObject^>(this, &GameObject::descendantAdded));
}

GameObject::GameObject(System::String^ n, Engine::Internal::Components::Transform^ transform, Engine::Internal::Components::ObjectType t, String^ tag, Engine::Components::Layer^ layer)
{
	this->coroutines = gcnew List<System::Collections::IEnumerator^>();
	this->childs = gcnew cli::array<GameObject^>(0);
	this->active = true;
	this->memberIsProtected = false;
	this->name = n;
	this->transform = transform;
	this->type = t;
	this->viewSpace = ViewSpace::V3D;

	this->InstanceType = gcnew Reflectable::ReflectableType();
	this->InstanceType->SetType(this->GetType());

	this->lastTransform = nullptr;
	layerMask = layer;

	if (tag == nullptr)
		tag = "";

	this->tag = tag;
	
#ifdef USE_BULLET_PHYS
	this->collisionShape = new Engine::Native::CollisionShape(this);
#endif
	// EVENT CREATION \\

	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();
	this->onChildAdded = gcnew Engine::Scripting::Events::Event();
	this->onChildRemoved = gcnew Engine::Scripting::Events::Event();
	this->onDescendantAdded = gcnew Engine::Scripting::Events::Event();

	this->onDescendantAdded->connect(gcnew Action<GameObject^>(this, &GameObject::descendantAdded));

	activeToggle = this->active;
}

void GameObject::setParent(GameObject^ object)
{
	if (object == nullptr)
		return;

	if (this->transform->parent != nullptr)
	{
		String^ TUID = this->transform->GetParent()->GetUID();
		Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(TUID)->onChildRemoved->raiseExecution(gcnew cli::array<System::Object^> { this });
	}

	transform->setParent(object->transform);

	object->onChildAdded->raiseExecution(gcnew cli::array<System::Object^> { this });
	object->onDescendantAdded->raiseExecution(gcnew cli::array<System::Object^> { this });
}

Engine::Internal::Components::Transform^ GameObject::getTransform()
{
	return transform;
}

String^ GameObject::getTag()
{
	return tag;
}

void GameObject::setTag(String^ tag)
{
	this->tag = tag;
}

void GameObject::OnPropChanged()
{
	if (activeToggle != active)
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "active", active, activeToggle });
	}

	if (lastTransform == nullptr)
	{
		lastTransform = gcnew Engine::Internal::Components::Transform(transform->position, transform->rotation, transform->scale, transform->parent);
		return;
	}

	if (!transform->position.Equals(lastTransform->position))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "position", transform->position, lastTransform->position });
		MoveChildren(this, this->childs);

		lastTransform = gcnew Engine::Internal::Components::Transform(transform->position, transform->rotation, transform->scale, transform->parent);
	}
	else if (!transform->rotation.Equals(lastTransform->rotation))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "rotation", transform->rotation, lastTransform->rotation });

		lastTransform = gcnew Engine::Internal::Components::Transform(transform->position, transform->rotation, transform->scale, transform->parent);
	}
	else if (!transform->scale.Equals(lastTransform->scale))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "scale", transform->scale, lastTransform->scale });

		lastTransform = gcnew Engine::Internal::Components::Transform(transform->position, transform->rotation, transform->scale, transform->parent);
	}
	else if ((lastTransform != nullptr && lastTransform->parent != nullptr) && (transform->parent != nullptr) && (!transform->parent->GetUID()->Equals(lastTransform->parent->GetUID())))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "parent", transform->parent, lastTransform->parent });

		lastTransform = gcnew Engine::Internal::Components::Transform(transform->position, transform->rotation, transform->scale, transform->parent);
	}
}

void fixChilds(GameObject^ root)
{
	for (int x = 0; x < root->childs->Length; x++)
	{
		GameObject^% obj = root->childs[x];

		if (obj->GetType() != obj->InstanceType->getTypeReference())
		{
			Engine::Scripting::ObjectManager::singleton()->Destroy(obj);
			GameObject^ newInstance = (GameObject^)Cast::Deserialzable((System::Object^%)obj, obj->InstanceType->getTypeReference());
			root->childs[x] = newInstance;
			Engine::Scripting::ObjectManager::singleton()->Instantiate(newInstance);
		}
		else
			continue;
	}
}

void Engine::Internal::Components::GameObject::Awake()
{
}

void GameObject::Start()
{
}

void GameObject::GameUpdate()
{
	try
	{
		/*
#ifdef USE_BULLET_PHYS
		if (!collisionObjectInitialized && !getCollider(this)->hasCollisionObject())
		{
			getCollider(this)->createCollisionShape(NativeSingleton<Engine::EngineObjects::Physics::Native::NativePhysicsService*>::Get()->getCollisionShapeForBox(1, 1, 1));
			getCollider(this)->createBulletObject(false);

			collisionObjectInitialized = true;
		}
#endif
		*/

		int layerId = 0;

		if (this->layerMask != nullptr)
			layerId = this->layerMask->layerMask;

		this->layerMask = Engine::Scripting::LayerManager::GetLayerFromId(layerId);

		OnPropChanged();
		this->childs = GetChildren();

		if (!active)
		{
			if (activeToggle)
			{
				activeToggle = false;
				OnInactive();
			}
			return;
		}
		else
		{
			if (!activeToggle)
			{
				activeToggle = true;
				OnActive();
			}
		}

		auto coroutinesCpy = coroutines->ToArray();

		if (coroutinesCpy->Length > 0)
		{
			for (int x = coroutinesCpy->Length; x > 0; x--)
			{
				auto coroutine = coroutinesCpy[x-1];

				if (!coroutine->MoveNext())
					coroutines->RemoveAt(x-1);
			}
		}

		HookUpdate();

		if (EngineState::PlayMode == true)
		{
			Update();
		}
		else
		{
			auto method = GetType()->GetMethod("Update");

			if (!method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false))
			{
				return;
			}

			Update();
		}
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
		printError(ex->StackTrace);
	}
}

void GameObject::GameDraw()
{
	if (!active)
		return;

	Draw();
}

void GameObject::GameDrawGizmos()
{
	if (!active)
		return;

	DrawGizmo();
}


void GameObject::GameDrawImGUI()
{
	if (!active)
		return;


	if (EngineState::PlayMode == true)
	{
		DrawImGUI();
	}
	else
	{
		auto method = GetType()->GetMethod("DrawImGUI");

		if (!method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false))
		{
			return;
		}

		DrawImGUI();
	}
}

generic <class T>
T GameObject::ToObjectType()
{
	try
	{
		return Cast::Dynamic<T>(this);
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}
}

generic <class T>
T Engine::Internal::Components::GameObject::as()
{
	if (this->IsA<T>())
		return (T)this;
	else
		return T();
}

generic <class T>
bool Engine::Internal::Components::GameObject::isA()
{
	return (this->GetType() == T::typeid || this->GetType()->IsSubclassOf(T::typeid));
}

generic <class T>
T Engine::Internal::Components::GameObject::FindFirstChild()
{
	for each (GameObject^ child in this->GetChildren())
	{
		if (child->IsA<T>())
			return (T)child;
	}

	return T();
}

generic <class T>
T Engine::Internal::Components::GameObject::FindFirstSibling()
{
	if (Parent == nullptr)
		return T();

	for each (GameObject ^ child in this->Parent->GetChildren())
	{
		if (child->IsA<T>())
			return (T)child;
	}

	return T();
}

generic <class T>
T Engine::Internal::Components::GameObject::FindFirstObjectOfType()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetFirstObjectOfType<T>();
}

generic <class T>
T GameObject::ToGenericType()
{
	return (T)this;
}

System::Object^ GameObject::CastToType(Type^ T, bool useConvert)
{
	try
	{
		if (useConvert)
		{
			return System::Convert::ChangeType(this, T);
		}
		else
		{
			auto baseMethod = GetType()->GetMethod("ToGenericType");
			auto genericMethod = baseMethod->MakeGenericMethod(T);

			return genericMethod->Invoke(this, nullptr);
		}
	}
	catch (Exception^ ex)
	{
		printError(ex->Message);
	}

	return nullptr;
}

void Engine::Internal::Components::GameObject::Destroy()
{
#ifdef USE_BULLET_PHYS
	if(collisionShape != nullptr)
		delete collisionShape;
#endif
}


cli::array<GameObject^>^ Engine::Internal::Components::GameObject::GetDescendants()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetDescendantsOf(this)->ToArray();
}

cli::array<GameObject^>^ GameObject::GetChildren()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetChildrenOf(this)->ToArray();
}

GameObject^ GameObject::GetChild(int index)
{
	try
	{
		return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetChildrenOf(this)[index];
	}
	catch(Exception^ ex)
	{
		printError(ex->Message);
		return nullptr;
	}
}

GameObject^ GameObject::GetChild(String^ childName)
{
	auto children = GetChildren();

	for each (GameObject^ child in children)
	{
		if (child->name == childName)
			return child;
	}

	return nullptr;
}

GameObject^ GameObject::InstantiateChild(GameObject^ instance)
{
	instance->setParent(this);
	Singleton<Engine::Scripting::ObjectManager^>::Instance->Instantiate(instance);
	return instance;
}

void Engine::Internal::Components::GameObject::LaunchCoroutine(System::Collections::IEnumerator^ coroutine)
{
	this->coroutines->Add(coroutine);
}

void Engine::Internal::Components::GameObject::RemoveCoroutine(System::Collections::IEnumerator^ coroutine)
{
	this->coroutines->Remove(coroutine);
}

void Engine::Internal::Components::GameObject::RemoveCoroutine(int index)
{
	this->coroutines->RemoveAt(index);
}

void Engine::Internal::Components::GameObject::CleanCoroutines()
{
	this->coroutines->Clear();
}

bool Engine::Internal::Components::GameObject::isA(System::Type^ type)
{
	return (this->GetType() == type || this->GetType()->IsSubclassOf(type));
}

void Engine::Internal::Components::GameObject::Destroy(GameObject^ instance)
{
	Singleton<Engine::Scripting::ObjectManager^>::Instance->Destroy(instance);
}

GameObject^ GameObject::Instantiate(GameObject^ instance)
{
	Singleton<Engine::Scripting::ObjectManager^>::Instance->Instantiate(instance);
	return instance;
}

GameObject^ GameObject::Instantiate(GameObject^ instance, Transform^ parent)
{
	instance->transform->setParent(parent);
	Singleton<Engine::Scripting::ObjectManager^>::Instance->Instantiate(instance);
	return instance;
}

GameObject^ Engine::Internal::Components::GameObject::FindFirstObjectByName(System::String^ name)
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetFirstObjectByName(name);
}

GameObject^ Engine::Internal::Components::GameObject::FindFirstObjectByTag(System::String^ tag)
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetFirstObjectByTag(tag);
}

Engine::Internal::Components::ObjectType GameObject::GetObjectType()
{
	return this->type;
}

GameObject^ GameObject::Parent::get()
{
	return (GameObject^)Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectFromTransform(this->transform->getParent());
}

void GameObject::Parent::set(GameObject^ arg)
{
	return this->setParent(arg);
}