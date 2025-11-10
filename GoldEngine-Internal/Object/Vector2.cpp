#include "../Includes.h"
#include "../ManagedIncludes.h"
#include "../GlIncludes.h"
#include "Vector2.h"

Engine::Components::Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

RAYLIB::Vector2 Engine::Components::Vector2::toNative()
{
	return { this->x, this->y };
}

Engine::Components::Vector2 Engine::Components::Vector2::add(Engine::Components::Vector2 other)
{
	return Engine::Components::Vector2(
		x + other.x,
		y + other.y
	);
}

Engine::Components::Vector2 Engine::Components::Vector2::sub(Engine::Components::Vector2 other)
{
	return Engine::Components::Vector2(
		x - other.x, 
		y - other.y
	);
}

Engine::Components::Vector2::operator Engine::Components::Vector2(System::Numerics::Vector2 v2)
{
	return Engine::Components::Vector2(v2.X, v2.Y);
}

Engine::Components::Vector2::operator System::Numerics::Vector2(Engine::Components::Vector2 v2)
{
	return System::Numerics::Vector2(v2.x, v2.y);
}

Engine::Components::Vector2 Engine::Components::Vector2::multiply(Engine::Components::Vector2 other)
{
	return Engine::Components::Vector2(
		x * other.x,
		y * other.y
	);
}

Engine::Components::Vector2 Engine::Components::Vector2::divide(Engine::Components::Vector2 other)
{
	return Engine::Components::Vector2(
		x / other.x,
		y / other.y
	);
}