#pragma once

#include "../Reflection/ReflectedType.h"

#ifdef USE_BULLET_PHYS
namespace Engine::Native
{
	class CollisionShape;
}
#endif

namespace Engine::Internal::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
		public ref class GameObject
	{
	private:
		bool isDisposed = false;
#ifdef USE_BULLET_PHYS
		[Newtonsoft::Json::JsonIgnoreAttribute]
		Engine::Native::CollisionShape* collisionShape = nullptr;
		Engine::Native::CollisionShape* originalCollisionShape = nullptr;

		bool collisionObjectInitialized = false;
#endif
		Engine::Internal::Components::Transform^ lastTransform;
		[Newtonsoft::Json::JsonPropertyAttribute]
		bool memberIsProtected;
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Internal::Components::ObjectType type;

		System::Collections::Generic::List<
			System::Collections::Generic::Stack<
				System::Collections::IEnumerator^>^>^ coroutines;

		bool updateExecutesInEditMode = false;
		bool drawImGuiExecutesInEditMode = false;
		bool initialized = false;
		bool activeToggle = false;

		System::Collections::Generic::List<
			Engine::Internal::Components::GameObject^
		>^ cachedChildren;

	public:
		bool active;

		System::String^ name;
		Engine::Internal::Components::Transform^ transform;

		Engine::Internal::Components::ViewSpace viewSpace;
		Engine::Components::Layer^ layerMask;
		System::String^ tag;

		[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onPropertyChanged;
		[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onChildAdded;
		[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onChildRemoved;
		[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onDescendantAdded;

		Reflectable::ReflectableType^ InstanceType;

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property GameObject^ Parent 
		{
			GameObject^ get();
			void set(GameObject^ arg);
		}

	public:
		[Newtonsoft::Json::JsonConstructorAttribute]
		GameObject();
		GameObject(System::String^ n, Engine::Internal::Components::Transform^ transform, Engine::Internal::Components::ObjectType t, String^ tag, Engine::Components::Layer^ layer);
		virtual ~GameObject();
		!GameObject();

#ifdef USE_BULLET_PHYS
	internal:
		Engine::Native::CollisionShape* getCollisionShape() { return collisionShape; }

		void overrideCollisionShape(Engine::Native::CollisionShape* collisionShape);
		void restoreCollisionShape();

		void createCollisionShape();
#endif

	internal:
		void InitializeObject();
		// vmethods
		// init functions (used by reflector & scene loader).
		// internal 
		virtual void Init(GameObject^ object) {}
		virtual void Init() {}
		virtual void Init(array<System::Object^>^ params) {}
		virtual void Init(System::Object^ object) {}
		virtual void Setup() {}
		virtual void HookUpdate() {}

	public:

		// object methods
		virtual void Awake();
		virtual void Start();
		virtual void PhysicsUpdate() {}
		virtual void Update() { }
		virtual void Draw() {}
		virtual void DrawGUI() {}
		virtual void DrawGizmo() {}
		virtual void DrawImGUI() {}

		virtual void OnActive() {}
		virtual void OnInactive() {}

		virtual void OnCollided(GameObject^ collider) {}
		virtual void OnTriggered(GameObject^ collider) {}

		virtual void OnCollisionEnter(GameObject^ collider) {}
		virtual void OnTriggerEnter(GameObject^ collider) {}

		virtual void OnCollisionStay(GameObject^ collider) {}
		virtual void OnTriggerStay(GameObject^ collider) {}

		virtual void OnCollisionExit(GameObject^ collider) {}
		virtual void OnTriggerExit(GameObject^ collider) {}


		// engine methods

	private:
		void descendantAdded(GameObject^ descendant);
		void OnPropChanged();
		void MoveChildren();
		void RotateChildren();
		void ScaleChildren();

	internal:
		void GameUpdate();
		void GameDraw();
		void GameDrawGizmos();
		void GameDrawImGUI();

		// defined
	public:
		bool isProtected()
		{
			return memberIsProtected;
		}

		void protectMember()
		{
			memberIsProtected = true;
		}

		void unprotectMember()
		{
			memberIsProtected = false;
		}

		String^ getTag();
		void setTag(String^);
		Transform^ getTransform();
		void setParent(GameObject^ object);

		String^ GetTag() { return getTag(); }

		void SetTag(String^ arg) { this->setTag(arg); }
		Transform^ GetTransform() { return getTransform(); }
		void SetParent(GameObject^ arg) { this->setParent(arg); }

		Engine::Internal::Components::ObjectType GetObjectType();

		generic <class T>
		T ToGenericType();

		generic <class T>
		T ToObjectType();

		System::Object^ CastToType(Type^ T, bool useConvert);

		virtual void Destroy();

		auto ToDerivate()
		{
			return System::Convert::ChangeType(this, this->GetType());
		}

		System::Type^ GetRuntimeType()
		{
			return this->GetType();
		}

		void SetActive(bool active)
		{
			this->active = active;
		}

		void SetLayerMask(Engine::Components::Layer^ layerMask)
		{
			this->layerMask = layerMask;
		}

		cli::array<GameObject^>^ GetDescendants();
		cli::array<GameObject^>^ GetChildren();

		GameObject^ GetChild(int index);
		GameObject^ GetChild(String^ name);

		GameObject^ InstantiateChild(GameObject^ instance);

		void LaunchCoroutine(System::Collections::IEnumerator^ coroutine);
		void StopCoroutine(System::Collections::IEnumerator^ coroutine);
		void StopAllCoroutines();

		generic <class T> T as();
		generic <class T> T As() { return as<T>(); }

		generic <class T> bool isA();
		generic <class T> bool IsA() { return isA<T>(); }
		bool isA(System::Type^ type);
		bool IsA(System::Type^ type) { return isA(type); }

		generic <class T>
		T FindFirstChild();
		generic <class T>
		T FindFirstSibling();

		static void Destroy(GameObject^ instance);
		static GameObject^ Instantiate(GameObject^ instance);
		static GameObject^ Instantiate(GameObject^ instance, Transform^ parent);

		generic <class T>
		static T FindFirstObjectOfType();
		static GameObject^ FindFirstObjectByName(System::String^ name);
		static GameObject^ FindFirstObjectByTag(System::String^ tag);
		static cli::array<GameObject^>^ GetObjects();

		bool IsDisposed();
internal:
	Engine::Internal::Components::Transform^ GetLastFrameTransform();
};
}