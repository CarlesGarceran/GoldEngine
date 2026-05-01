#pragma once

#include "../Includes.h"
#include "../GlIncludes.h"
#include "../CastToNative.h"
#include "ObjectType.h"
#include "ViewSpace.h"
#include "Layer.h"
#include "Transform.h"
#include "LayerManager.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix.h"

using namespace System;

namespace Engine::Internal::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Transform sealed
	{
	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		System::Guid uid;
		[Newtonsoft::Json::JsonPropertyAttribute]
		System::Guid parentUid;
	
		Transform^ parentCache;

	public:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Internal::Components::Transform^ parent
		{
			Engine::Internal::Components::Transform^ get();
			void set(Engine::Internal::Components::Transform^);
		}
		// worldspace
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3 position
		{
			Engine::Components::Vector3 get();
			void set(Engine::Components::Vector3 position);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Quaternion rotation
		{
			Engine::Components::Quaternion get();
			void set(Engine::Components::Quaternion position);
		}

		// localspace
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3 localPosition
		{
			Engine::Components::Vector3 get();
			void set(Engine::Components::Vector3 position);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Quaternion localRotation
		{
			Engine::Components::Quaternion get();
			void set(Engine::Components::Quaternion position);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
			property Engine::Components::Vector3 forward
		{
			Engine::Components::Vector3 get();
		}


		[Newtonsoft::Json::JsonIgnoreAttribute]
			property Engine::Components::Vector3 right
		{
			Engine::Components::Vector3 get();
		}


		[Newtonsoft::Json::JsonIgnoreAttribute]
			property Engine::Components::Vector3 up
		{
			Engine::Components::Vector3 get();
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3 scale {
			Engine::Components::Vector3 get();
			void set(Engine::Components::Vector3);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
			property Engine::Components::Vector3 localScale {
			Engine::Components::Vector3 get();
			void set(Engine::Components::Vector3);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Matrix16^ WorldMatrix {
			Engine::Components::Matrix16^ get();
		}

		// CONSTANT VECTORS
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly Engine::Components::Vector3 FRONT = Engine::Components::Vector3(0, 0, 1);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly  Engine::Components::Vector3 TOP = Engine::Components::Vector3(0, 1, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly  Engine::Components::Vector3 RIGHT = Engine::Components::Vector3(1, 0, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly  Engine::Components::Vector3 BACK = Engine::Components::Vector3(0, 0, -1);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly  Engine::Components::Vector3 BOTTOM = Engine::Components::Vector3(0, -1, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			static initonly  Engine::Components::Vector3 LEFT = Engine::Components::Vector3(-1, 0, 0);

	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Components::Vector3 worldPosition = Engine::Components::Vector3::Zero();
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Components::Quaternion worldRotation = Engine::Components::Quaternion::Identity;
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Components::Vector3 worldScale = Engine::Components::Vector3::One;

	public:
		Transform(Engine::Components::Vector3 position, Engine::Components::Quaternion rotation, Engine::Components::Vector3 scale, Transform^ parent);

	public:
		System::Guid GetGUID();
		System::String^ GetUID();

		Transform^ getParent();
		Transform^ GetParent() { return getParent(); }

		Engine::Components::Vector3 InverseTransformPoint(Engine::Components::Vector3 worldPoint);

		generic <class T>
		T GetObject();
		System::Object^ GetObject();


		void SetUID(System::String^ uid);
		void SetGUID(System::Guid uid);

	internal:
		void setParent(Transform^);
		void SetParent(Transform^ parent) { return setParent(parent); }
	};
}