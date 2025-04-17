#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Vector2 : Engine::Interfaces::IInstantiable<Vector2^>
	{
	public:
		float x, y;

	public:
		[[JsonConstructorAttribute]]
		Vector2(float x, float y);
		Vector2();

		RAYLIB::Vector2 toNative();

	public:
		static Vector2^ lerp(Vector2^ origin, Vector2^ target, float interpolate)
		{
			auto result = RAYMATH::Vector2Lerp(origin->toNative(), target->toNative(), interpolate);

			return gcnew Vector2(result.x, result.y);
		}

		static Vector2^ zero()
		{
			return gcnew Vector2(0, 0);
		}

		static Vector2^ Zero()
		{
			return zero();
		}

		System::Numerics::Vector2^ toNumericsVector2()
		{
			return gcnew System::Numerics::Vector2(this->x, this->y);
		}

		static Vector2^ create(RAYLIB::Vector2 vector)
		{
			return gcnew Engine::Components::Vector2(vector.x, vector.y);
		}

		static Vector2^ create(float* vector)
		{
			return gcnew Engine::Components::Vector2(vector[0], vector[1]);
		}

		static Vector2^ New(float x, float y)
		{
			return gcnew Vector2(x, y);
		}

		static Vector2^ Create()
		{
			return gcnew Vector2();
		}

		static Vector2^ New()
		{
			return gcnew Vector2();
		}

		Vector2^ add(Vector2^ origin);
		Vector2^ add(float x, float y)
		{
			return add(gcnew Engine::Components::Vector2(x, y));
		}
		Vector2^ add(float x)
		{
			return add(x, x);
		}

		Vector2^ multiply(Vector2^ origin);
		Vector2^ multiply(float x, float y)
		{
			return multiply(gcnew Engine::Components::Vector2(x, y));
		}
		Vector2^ multiply(float x)
		{
			return multiply(x, x);
		}

		Vector2^ divide(Vector2^ origin);
		Vector2^ divide(float x, float y)
		{
			return divide(gcnew Engine::Components::Vector2(x, y));
		}
		Vector2^ divide(float x)
		{
			return divide(x, x);
		}

		Vector2^ sub(Vector2^ origin);
		Vector2^ sub(float x, float y)
		{
			return sub(gcnew Engine::Components::Vector2(x, y));
		}
		Vector2^ sub(float x)
		{
			return sub(x, x);
		}

		void copy(const Vector2^ inVec);

		bool Equals(Vector2^ value) override
		{
			return ((this->x == value->x) && (this->y == value->y));
		}

		static Vector2^ add(Vector2^ left, Vector2^ right)
		{
			return gcnew Vector2(left->x + right->x, left->y + right->y);
		}

		static Vector2^ sub(Vector2^ left, Vector2^ right)
		{
			return gcnew Vector2(left->x - right->x, left->y - right->y);
		}

		static Vector2^ multiply(Vector2^ left, Vector2^ right)
		{
			return gcnew Vector2(left->x * right->x, left->y * right->y);
		}

		static Vector2^ multiply(Vector2^ left, float value)
		{
			return gcnew Vector2(left->x * value, left->y * value);
		}

		static Vector2^ divide(Vector2^ left, Vector2^ right)
		{
			return gcnew Vector2(left->x / right->x, left->y / right->y);
		}

		static Vector2^ divide(Vector2^ left, float value)
		{
			return gcnew Vector2(left->x / value, left->y / value);
		}


		Engine::Components::Vector2^ operator+(Vector2^ other)
		{
			return Vector2::add(this, other);
		}

		Engine::Components::Vector2^ operator-(Vector2^ other)
		{
			return Vector2::sub(this, other);
		}

		Engine::Components::Vector2^ operator*(Vector2^ other)
		{
			return Vector2::multiply(this, other);
		}

		Engine::Components::Vector2^ operator/(Vector2^ other)
		{
			return Vector2::divide(this, other);
		}


		float magnitude()
		{
			return sqrt((x * x) + (y * y));
		}
	};
}