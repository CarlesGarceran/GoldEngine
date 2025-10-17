#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public value class Vector3
	{
	private:
		[Newtonsoft::Json::JsonPropertyAttribute]
		float _x, _y, _z;

	public:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property float x
		{
			float get() { return _x; }
			void set(float value) { _x = value; }
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property float y
		{
			float get() { return _y; }
			void set(float value) { _y = value; }
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property float z
		{
			float get() { return _z; }
			void set(float value) { _z = value; }
		}

		property float Magnitude
		{
			float get() { return this->magnitude(); }
		}

	public:
		Vector3(float x, float y, float z);

		Engine::Components::Vector2 toVector2();
		System::Numerics::Vector3 toNumericsVector3();

		void Set(float x, float y, float z);
		void Set(RAYLIB::Vector3 v);
		void Set(float* v);

		RAYLIB::Vector3 toNative();

		RAYLIB::Color toColor();

		Vector3 add(Vector3 origin);
		Vector3 add(float x, float y, float z) 
		{
			return add(Engine::Components::Vector3(x, y, z));
		}
		Vector3 add(float x)
		{
			return add(x, x, x);
		}

		Vector3 multiply(Vector3 origin);
		Vector3 multiply(float x, float y, float z)
		{
			return multiply(Engine::Components::Vector3(x, y, z));
		}
		Vector3 multiply(float x)
		{
			return multiply(x, x, x);
		}

		Vector3 divide(Vector3 origin);
		Vector3 divide(float x, float y, float z) 
		{
			return divide(Engine::Components::Vector3(x, y, z));
		}
		Vector3 divide(float x)
		{
			return divide(x, x, x);
		}

		Vector3 sub(Vector3 origin);
		Vector3 sub(float x, float y, float z)
		{
			return sub(Engine::Components::Vector3(x, y, z));
		}
		Vector3 sub(float x)
		{
			return sub(x, x, x);
		}

		// ANGLE \\

		static float angle(Vector3 left, Vector3 right);
		static float Angle(Vector3 left, Vector3 right) { return angle(left, right); }

		inline float Angle(Vector3 right) { return angle(*this, right); }

		// CROSS PRODUCT \\

		static Vector3 cross(Vector3 left, Vector3 right);
		static Vector3 Cross(Vector3 left, Vector3 right) { return cross(left, right); }

		inline Engine::Components::Vector3 cross(Vector3 target) { return cross(*this, target); }
		inline Engine::Components::Vector3 Cross(Vector3 target) { return cross(*this, target); }

		// LERP \\

		static Vector3 lerp(Vector3 origin, Vector3 target, float interpolate);
		static Vector3 Lerp(Vector3 origin, Vector3 target, float interpolate) { return lerp(origin, target, interpolate); }

		inline void lerp(Vector3 target, float interpolate) { Lerp(target, interpolate); }
		inline void Lerp(Vector3 target, float interpolate);

		static Vector3 zero()
		{
			return Vector3(0, 0, 0);
		}

		static Vector3 Zero()
		{
			return zero();
		}

		static Vector3 add(Vector3 left, Vector3 right)
		{
			return Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
		}

		static Vector3 sub(Vector3 left, Vector3 right)
		{
			return Vector3(left.x - right.x, left.y - right.y, left.z - right.z);
		}

		static Vector3 multiply(Vector3 left, Vector3 right)
		{
			return Vector3(left.x * right.x, left.y * right.y, left.z * right.z);
		}

		static Vector3 divide(Vector3 left, Vector3 right)
		{
			return Vector3(left.x / right.x, left.y / right.y, left.z / right.z);
		}

		static Vector3 divide(Vector3 left, float right)
		{
			return Vector3(left.x / right, left.y / right, left.z / right);
		}

		static Vector3 multiply(Vector3 left, float right)
		{
			return Vector3(left.x * right, left.y * right, left.z * right);
		}

		static Vector3 Create()
		{
			return Vector3();
		}

		static Vector3 New()
		{
			return Vector3();
		}

		static Vector3 New(float x, float y, float z)
		{
			return Vector3(x,y,z);
		}

		static Vector3 create(RAYLIB::Vector3 vec)
		{
			return Vector3(vec.x, vec.y, vec.z);
		}

		static Vector3 create(float vec[])
		{
			return Vector3(vec[0], vec[1], vec[2]);
		}

		static float Dot(Engine::Components::Vector3 left, Engine::Components::Vector3 right);
		static float DistanceScalar(Engine::Components::Vector3 left, Engine::Components::Vector3 right);
		static Engine::Components::Vector3 Direction(Engine::Components::Vector3 from, Engine::Components::Vector3 to);
		static Engine::Components::Vector3 DirectionNormalized(Engine::Components::Vector3 from, Engine::Components::Vector3 to);
		Engine::Components::Vector3 Normalized();
		static float Distance(Vector3 left, Vector3 right);

		bool Equals(Vector3 value) override
		{
			return ((this->x == value.x) && (this->y == value.y) && (this->z == value.z));
		}

		/*
		Engine::Components::Vector3 operator+(Vector3 other)
		{
			return Vector3::add(*this, other);
		}

		Engine::Components::Vector3 operator-(Vector3 other)
		{
			return Vector3::sub(*this, other);
		}

		Engine::Components::Vector3 operator*(Vector3 other)
		{
			return Vector3::multiply(*this, other);
		}

		Engine::Components::Vector3 operator/(Vector3 other)
		{
			return Vector3::divide(*this, other);
		}
		*/

		float magnitude()
		{
			return sqrt((x * x) + (y * y) + (z * z));
		}

		static Engine::Components::Vector3 operator+(Vector3 left, Vector3 right)
		{
			return Vector3::add(left, right);
		}

		static Engine::Components::Vector3 operator-(Vector3 left, Vector3 right)
		{
			return Vector3::sub(left, right);
		}

		static Engine::Components::Vector3 operator*(Vector3 left, Vector3 right)
		{
			return Vector3::multiply(left, right);
		}

		static Engine::Components::Vector3 operator*(Vector3 left, float right)
		{
			return Vector3::multiply(left, right);
		}

		static  Engine::Components::Vector3 operator/(Vector3 left, Vector3 right)
		{
			return Vector3::divide(left, right);
		}

		static  Engine::Components::Vector3 operator/(Vector3 left, float right)
		{
			return Vector3::divide(left, right);
		}

		static bool operator==(Vector3 left, Vector3 right)
		{
			return (left.x == right.x && left.y == right.y && left.z == right.z);
		}
	};

}