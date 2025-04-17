#pragma once

#include "../Reflection/ReflectedType.h"

namespace Engine::Internal::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class GameObject
	{
	private:
#ifdef USE_BULLET_PHYS
		void* collisionShape;
		bool collisionObjectInitialized = false;
#endif
		Engine::Internal::Components::Transform^ lastTransform;
		[Newtonsoft::Json::JsonPropertyAttribute]
		bool memberIsProtected;
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Internal::Components::ObjectType type;

	public:
		bool active;

		System::String^ name;
		Engine::Internal::Components::Transform^ transform;

		Engine::Internal::Components::ViewSpace viewSpace;
		Engine::Components::Layer^ layerMask;
		System::String^ tag;

		[Newtonsoft::Json::JsonIgnoreAttribute]
			System::Collections::Generic::List<GameObject^>^ childs;

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

	private:
		~GameObject()
		{
			childs->Clear();
			onPropertyChanged->disconnectAll();
			onChildAdded->disconnectAll();
			onChildRemoved->disconnectAll();
			onDescendantAdded->disconnectAll();
			
#ifdef USE_BULLET_PHYS
			delete collisionShape;
#endif
			delete name;
			delete transform;
			delete layerMask;
			delete tag;
			delete lastTransform;
			delete childs;
			delete onPropertyChanged;
			delete onChildAdded;
			delete onChildRemoved;
			delete onDescendantAdded;
			delete InstanceType;
		}

#ifdef USE_BULLET_PHYS
	public:
		void* getCollisionShape() { return collisionShape; }
		void createCollisionShape();
#endif

	public:
		// vmethods
		// init functions (used by reflector & scene loader).
		virtual void Init(GameObject^ object) {}
		virtual void Init() {}
		virtual void Init(array<System::Object^>^ params) {}
		virtual void Init(System::Object^ object) {}
		virtual void Setup() {}

		// object methods
		virtual void Start();
		virtual void PhysicsUpdate() {}
		virtual void Update() { }
		virtual void Draw() {}
		virtual void DrawGUI() {}
		virtual void DrawGizmo() {}
		virtual void DrawImGUI() {}

		virtual void OnActive() {}
		virtual void OnUnactive() {}

		virtual void OnCollisionEnter(GameObject^ other) {}

		// internal methods
	protected:
		virtual void HookUpdate() {}

		// engine methods

	private:
		void descendantAdded(GameObject^ descendant);
		void OnPropChanged();

	public:
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

		virtual void Destroy()
		{
			delete this;
		}

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

		System::Collections::Generic::List<GameObject^>^ GetChildren();
		GameObject^ GetChild(int index);
		GameObject^ GetChild(String^ name);

		GameObject^ InstantiateChild(GameObject^ instance);

		static GameObject^ Instantiate(GameObject^ instance);
		static GameObject^ Instantiate(GameObject^ instance, Transform^ parent);

		/*
#pragma region IConvertible_Implementation

		virtual System::TypeCode GetTypeCode()
		{
			return System::TypeCode();
		}

		virtual bool ToBoolean(System::IFormatProvider^ provider)
		{
			return false;
		}

		virtual wchar_t ToChar(System::IFormatProvider^ provider)
		{
			return L'\0';
		}

		virtual signed char ToSByte(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual unsigned char ToByte(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual short ToInt16(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual unsigned short ToUInt16(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual int ToInt32(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual unsigned int ToUInt32(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual long long ToInt64(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual unsigned long long ToUInt64(System::IFormatProvider^ provider)
		{
			return 0;
		}

		virtual float ToSingle(System::IFormatProvider^ provider)
		{
			return 0.0f;
		}

		virtual double ToDouble(System::IFormatProvider^ provider)
		{
			return 0.0;
		}

		virtual System::Decimal ToDecimal(System::IFormatProvider^ provider)
		{
			return System::Decimal();
		}

		virtual System::DateTime ToDateTime(System::IFormatProvider^ provider)
		{
			return System::DateTime();
		}

		virtual System::String^ ToString(System::IFormatProvider^ provider)
		{
			return "Script";
		}

		virtual System::Object^ ToType(System::Type^ conversionType, System::IFormatProvider^ provider)
		{
			return System::Convert::ChangeType(this, conversionType);
		}

#pragma endregion
*/
};
}