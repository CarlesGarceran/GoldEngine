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

#ifdef USE_BULLET_PHYS

void Engine::Internal::Components::GameObject::setCollisionShape(Engine::Native::CollisionShape* shape)
{
	if (this->collisionShape == nullptr) return;

	if (this->collisionShape != shape)
	{
		delete this->collisionShape;
	}

	this->collisionShape = shape;
}

void GameObject::createCollisionShape()
{
	this->collisionShape = new Engine::Native::CollisionShape(this);
}

void Engine::Internal::Components::GameObject::InitializeObject()
{
	if (initialized) return;
	initialized = true;

#ifdef USE_BULLET_PHYS
	if (collisionShape == nullptr)
		createCollisionShape();
#endif

	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();
	this->onChildAdded = gcnew Engine::Scripting::Events::Event();
	this->onChildRemoved = gcnew Engine::Scripting::Events::Event();
	this->onDescendantAdded = gcnew Engine::Scripting::Events::Event();

	this->onDescendantAdded->connect(gcnew Action<GameObject^>(this, &GameObject::descendantAdded));

	auto method = GetType()->GetMethod("Update");
	updateExecutesInEditMode = method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false);

	method = GetType()->GetMethod("DrawImGUI");
	drawImGuiExecutesInEditMode = method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false);
}

#endif

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
	this->coroutines = gcnew System::Collections::Generic::List<System::Collections::Generic::Stack<System::Collections::IEnumerator^>^>();

	if(layerMask != nullptr)
		this->layerMask = Engine::Scripting::LayerManager::GetLayerFromId(layerMask->layerMask);
}

GameObject::GameObject(System::String^ n, Engine::Internal::Components::Transform^ transform, Engine::Internal::Components::ObjectType t, String^ tag, Engine::Components::Layer^ layer)
{
	this->coroutines = gcnew System::Collections::Generic::List<System::Collections::Generic::Stack<System::Collections::IEnumerator^>^>();
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
	createCollisionShape();
#endif
	// EVENT CREATION \\

	this->onPropertyChanged = gcnew Engine::Scripting::Events::Event();
	this->onChildAdded = gcnew Engine::Scripting::Events::Event();
	this->onChildRemoved = gcnew Engine::Scripting::Events::Event();
	this->onDescendantAdded = gcnew Engine::Scripting::Events::Event();

	this->onDescendantAdded->connect(gcnew Action<GameObject^>(this, &GameObject::descendantAdded));

	activeToggle = this->active;

	auto method = GetType()->GetMethod("Update");

	if (method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false))
	{
		updateExecutesInEditMode = true;
	}

	method = GetType()->GetMethod("DrawImGUI");

	if (method->IsDefined(Engine::Attributes::ExecuteInEditModeAttribute::typeid, false))
	{
		drawImGuiExecutesInEditMode = true;
	}
}

Engine::Internal::Components::GameObject::~GameObject()
{
	onPropertyChanged->disconnectAll();
	onChildAdded->disconnectAll();
	onChildRemoved->disconnectAll();
	onDescendantAdded->disconnectAll();

	this->!GameObject();
	GC::SuppressFinalize(this);
}

Engine::Internal::Components::GameObject::!GameObject()
{
#ifdef USE_BULLET_PHYS
	if (collisionShape != nullptr)
	{
		delete collisionShape;
		collisionShape = nullptr;
	}
#endif
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
		MoveChildren();
	}
	else if (!transform->rotation.Equals(lastTransform->rotation))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "rotation", transform->rotation, lastTransform->rotation });
	}
	else if (!transform->scale.Equals(lastTransform->scale))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "scale", transform->scale, lastTransform->scale });
	}
	else if ((lastTransform != nullptr && lastTransform->parent != nullptr) && (transform->parent != nullptr) && (!transform->parent->GetUID()->Equals(lastTransform->parent->GetUID())))
	{
		onPropertyChanged->raiseExecution(gcnew cli::array<System::Object^> { "parent", transform->parent, lastTransform->parent });
	}

	lastTransform->position = transform->position;
	lastTransform->rotation = transform->rotation;
	lastTransform->scale = transform->scale;
	lastTransform->parent = transform->parent;
}

void Engine::Internal::Components::GameObject::MoveChildren()
{
	Engine::Components::Vector3 delta = transform->position - lastTransform->position;

	for each (auto child in GetChildren())
	{
		child->transform->position += delta;
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

		for (int i = coroutines->Count - 1; i >= 0; i--)
		{
			auto stack = coroutines[i];

			if (stack->Count == 0)
			{
				coroutines->RemoveAt(i);
				continue;
			}

			auto top = stack->Peek();

			if (top->MoveNext())
			{
				Object^ yielded = top->Current;

				System::Collections::IEnumerator^ nested = dynamic_cast<System::Collections::IEnumerator^>(yielded);
				if (nested != nullptr)
				{
					stack->Push(nested);
				}
			}
			else
			{
				stack->Pop();

				if (stack->Count == 0)
				{
					coroutines->RemoveAt(i);
				}
			}
		}

		HookUpdate();

		if (EngineState::PlayMode == true)
		{
			Update();
		}
		else
		{
			if (updateExecutesInEditMode) Update();
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
		if(drawImGuiExecutesInEditMode) DrawImGUI();
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
	auto stack = gcnew System::Collections::Generic::Stack<System::Collections::IEnumerator^>();
	stack->Push(coroutine);
	coroutines->Add(stack);
}

void Engine::Internal::Components::GameObject::StopCoroutine(System::Collections::IEnumerator^ routine)
{
	if (routine == nullptr)
		return;

	for (int i = coroutines->Count - 1; i >= 0; i--)
	{
		auto stack = coroutines[i];

		if (stack->Count == 0)
		{
			coroutines->RemoveAt(i);
			continue;
		}

		System::Collections::IEnumerator^ root = nullptr;
		for each(System::Collections::IEnumerator^ e in stack)
			root = e;

		if (Object::ReferenceEquals(root, routine))
		{
			coroutines->RemoveAt(i);
			return;
		}
	}
}

void Engine::Internal::Components::GameObject::StopAllCoroutines()
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
	Transform^ _transform = this->transform->getParent();

	if (_transform == nullptr) return nullptr;

	return (GameObject^)Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectFromTransform(_transform);
}

void GameObject::Parent::set(GameObject^ arg)
{
	return this->setParent(arg);
}