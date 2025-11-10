#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public value class Vector2
	{
	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		float _x, _y;

	public:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property float x
		{
			float get() { return _x; }
			void set(float value) { this->_x = value; }
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property float y
		{
			float get() { return _y; }
			void set(float value) { this->_y = value; }
		}

		property float Magnitude
		{
			float get() { return this->magnitude(); }
		}

	public:
		Vector2(float x, float y);

		RAYLIB::Vector2 toNative();

	public:
		static Vector2 lerp(Engine::Components::Vector2 origin, Engine::Components::Vector2 target, float interpolate)
		{
			auto result = RAYMATH::Vector2Lerp(origin.toNative(), target.toNative(), interpolate);

			return Vector2(result.x, result.y);
		}

		static Vector2 zero()
		{
			return Vector2(0, 0);
		}

		static Vector2 Zero()
		{
			return zero();
		}

		System::Numerics::Vector2 toNumericsVector2()
		{
			return System::Numerics::Vector2(this->x, this->y);
		}

		static Vector2 create(RAYLIB::Vector2 vector)
		{
			return Engine::Components::Vector2(vector.x, vector.y);
		}

		static Vector2 create(float* vector)
		{
			return Engine::Components::Vector2(vector[0], vector[1]);
		}

		static Vector2 New(float x, float y)
		{
			return Engine::Components::Vector2(x, y);
		}

		static Vector2 Create()
		{
			return Vector2(0,0);
		}

		static Vector2 New()
		{
			return Vector2(0,0);
		}

		Vector2 add(Vector2 origin);
		Vector2 add(float x, float y)
		{
			return add(Engine::Components::Vector2(x, y));
		}
		Vector2 add(float x)
		{
			return add(x, x);
		}

		Vector2 multiply(Vector2 origin);
		Vector2 multiply(float x, float y)
		{
			return multiply(Engine::Components::Vector2(x, y));
		}
		Vector2 multiply(float x)
		{
			return multiply(x, x);
		}

		Vector2 divide(Vector2 origin);
		Vector2 divide(float x, float y)
		{
			return divide(Engine::Components::Vector2(x, y));
		}
		Vector2 divide(float x)
		{
			return divide(x, x);
		}

		Vector2 sub(Vector2 origin);
		Vector2 sub(float x, float y)
		{
			return sub(Engine::Components::Vector2(x, y));
		}
		Vector2 sub(float x)
		{
			return sub(x, x);
		}

		bool Equals(Vector2 value)
		{
			return (x == value.x) && (y == value.y);
		}

		virtual bool Equals(System::Object^ obj) override
		{
			if (obj == nullptr) return false;
			if (obj->GetType() != Vector2::typeid) return false;
			return Equals(safe_cast<Vector2>(obj));
		}

		static Vector2 add(Vector2 left, Vector2 right)
		{
			return Vector2(left.x + right.x, left.y + right.y);
		}

		static Vector2 sub(Vector2 left, Vector2 right)
		{
			return Vector2(left.x - right.x, left.y - right.y);
		}

		static Vector2 multiply(Vector2 left, Vector2 right)
		{
			return Vector2(left.x * right.x, left.y * right.y);
		}

		static Vector2 multiply(Vector2 left, float value)
		{
			return Vector2(left.x * value, left.y * value);
		}

		static Vector2 divide(Vector2 left, Vector2 right)
		{
			return Vector2(left.x / right.x, left.y / right.y);
		}

		static Vector2 divide(Vector2 left, float value)
		{
			return Vector2(left.x / value, left.y / value);
		}

		static Vector2 Direction(Vector2 from, Vector2 to)
		{
			return (to - from);
		}

		/*
		Engine::Components::Vector2 operator+(Vector2 other)
		{
			return Vector2::add(*this, other);
		}

		Engine::Components::Vector2 operator-(Vector2 other)
		{
			return Vector2::sub(*this, other);
		}

		Engine::Components::Vector2 operator*(Vector2 other)
		{
			return Vector2::multiply(*this, other);
		}

		Engine::Components::Vector2 operator/(Vector2 other)
		{
			return Vector2::divide(*this, other);
		}
		*/

		float magnitude()
		{
			return sqrt((x * x) + (y * y));
		}

		static bool operator==(Vector2 left, Vector2 right)
		{
			return (left.x == right.x && left.y == right.y);
		}

		static Engine::Components::Vector2 operator+(Vector2 left, Vector2 right)
		{
			return Vector2::add(left, right);
		}

		static Engine::Components::Vector2 operator-(Vector2 left, Vector2 right)
		{
			return Vector2::sub(left, right);
		}

		static Engine::Components::Vector2 operator*(Vector2 left, Vector2 right)
		{
			return Vector2::multiply(left, right);
		}

		static Engine::Components::Vector2 operator/(Vector2 left, Vector2 right)
		{
			return Vector2::divide(left, right);
		}

		static operator Engine::Components::Vector2(System::Numerics::Vector2 v2);
		static operator System::Numerics::Vector2(Engine::Components::Vector2 v2);
	};
}