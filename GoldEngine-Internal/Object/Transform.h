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

using namespace System;

namespace Engine::Internal::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Transform
	{
	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
			System::String^ uid;
	public:
		Engine::Internal::Components::Transform^ parent;
		// worldspace
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3^ position
		{
			Engine::Components::Vector3 ^ get();
			void set(Engine::Components::Vector3^ position);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3^ rotation
		{
			Engine::Components::Vector3 ^ get();
			void set(Engine::Components::Vector3^ position);
		}

		// localspace
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3^ localPosition
		{
			Engine::Components::Vector3 ^ get();
			void set(Engine::Components::Vector3^ position);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Engine::Components::Vector3^ localRotation
		{
			Engine::Components::Vector3 ^ get();
			void set(Engine::Components::Vector3^ position);
		}

		Engine::Components::Vector3^ scale;

		// CONSTANT VECTORS
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ forward = gcnew Engine::Components::Vector3(0, 0, 1);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ top = gcnew Engine::Components::Vector3(0, 1, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ right = gcnew Engine::Components::Vector3(1, 0, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ backward = gcnew Engine::Components::Vector3(0, 0, -1);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ bottom = gcnew Engine::Components::Vector3(0, -1, 0);
		[Newtonsoft::Json::JsonIgnoreAttribute]
			const Engine::Components::Vector3^ left = gcnew Engine::Components::Vector3(-1, 0, 0);

	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Components::Vector3^ worldPosition = Engine::Components::Vector3::Zero();
		[Newtonsoft::Json::JsonPropertyAttribute]
		Engine::Components::Vector3^ worldRotation = Engine::Components::Vector3::Zero();

	public:
		Transform(Engine::Components::Vector3^ position, Engine::Components::Vector3^ rotation, Engine::Components::Vector3^ scale, Transform^ parent);

	public:
		String^ GetUID();

		void setParent(Transform^);
		Transform^ getParent();

		void SetParent(Transform^ parent) { return setParent(parent); }
		Transform^ GetParent() { return getParent(); }

		generic <class T>
		T GetObject();
		System::Object^ GetObject();

		void SetUID(System::String^ uid);
	};
}