#pragma once

#pragma managed(push, off)
#include <btBulletCollisionCommon.h>
#pragma managed(pop)

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public value class Quaternion : Engine::Interfaces::IInstantiable<Quaternion>, System::ICloneable
	{
	public:
		float x, y, z, w;

		static initonly Quaternion Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

		property float Length
		{
			float get();
		}

		property float LengthSquared
		{
			float get();
		}

	public:
		[[JsonConstructorAttribute]]
		Quaternion(float x, float y, float z, float w);
		Quaternion(Engine::Components::Vector3 axis, float angleRadians);
		Quaternion(RAYLIB::Quaternion);

	public:
		bool IsIdentity();
		void Normalize();
		Engine::Components::Quaternion Normalized();
		Engine::Components::Quaternion Conjugate();
		Engine::Components::Quaternion Inverse();
		float Dot(Engine::Components::Quaternion q);

		Engine::Components::Vector3 Rotate(Engine::Components::Vector3 vector);
		
		static Engine::Components::Quaternion operator+(Engine::Components::Quaternion left, Engine::Components::Quaternion right);
		static Engine::Components::Quaternion operator-(Engine::Components::Quaternion left, Engine::Components::Quaternion right);
		static Engine::Components::Quaternion operator*(Engine::Components::Quaternion left, float scalar);
		static Engine::Components::Quaternion operator*(Engine::Components::Quaternion left, Engine::Components::Quaternion right);
		static Engine::Components::Vector3 operator*(Engine::Components::Vector3 left, Engine::Components::Quaternion right);
		static Engine::Components::Vector3 operator*(Engine::Components::Quaternion left, Engine::Components::Vector3 right);
		static Engine::Components::Quaternion operator/(Engine::Components::Quaternion left, Engine::Components::Quaternion right);
		static Engine::Components::Quaternion operator/(Engine::Components::Quaternion left, float scalar);


		static Engine::Components::Quaternion Lerp(Quaternion a, Quaternion b, float t);
		static Engine::Components::Quaternion NLerp(Quaternion a, Quaternion b, float t);
		static Engine::Components::Quaternion SLerp(Quaternion a, Quaternion b, float t);
		static Engine::Components::Vector3 Rotate(Engine::Components::Quaternion quaternion, Engine::Components::Vector3 vector);
		static Engine::Components::Quaternion LookRotation(Engine::Components::Vector3 forward, Engine::Components::Vector3 up);
		static Engine::Components::Quaternion FromAxis(Engine::Components::Vector3 axis, float angle);
		static Engine::Components::Quaternion FromAxis(RAYLIB::Vector3 axis, float angle);

		static Engine::Components::Quaternion FromEulerRadians(RAYLIB::Vector3 euler);
		static Engine::Components::Quaternion FromEulerRadians(Engine::Components::Vector3 euler);
		static Engine::Components::Quaternion FromEulerRadians(float x, float y, float z);

		static Engine::Components::Quaternion FromEulerAngles(RAYLIB::Vector3 euler);
		static Engine::Components::Quaternion FromEulerAngles(Engine::Components::Vector3 euler);
		static Engine::Components::Quaternion FromEulerAngles(float x, float y, float z);

		Engine::Components::Vector3 ToEuler();
		Engine::Components::Vector3 ToEulerRadians();
		Engine::Components::Vector3 ToEulerAngles();

		static bool operator==(Engine::Components::Quaternion left, Engine::Components::Quaternion right);
		static bool operator==(const Engine::Components::Quaternion& left, const Engine::Components::Quaternion& other);
		static bool operator!=(Engine::Components::Quaternion left, Engine::Components::Quaternion other);
		
		bool Equals(System::Object^ obj) override;
		virtual System::Object^ Clone();

		RAYLIB::Quaternion toNative();

		static operator Engine::Components::Vector3(Engine::Components::Quaternion quaternion); // Euler Angles Imlicit
		static operator System::Numerics::Quaternion(Engine::Components::Quaternion quaternion); // To Numerics Quaternion
	
	private:
		Engine::Components::Vector3 p_toEuler();
	};
}