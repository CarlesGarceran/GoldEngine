#include "../Includes.h"
#include "../ManagedIncludes.h"
#include "../GlIncludes.h"
#include "Vector2.h"

Engine::Components::Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

Engine::Components::Vector2::Vector2()
{
	this->x = 0.0f;
	this->y = 0.0f;
}

RAYLIB::Vector2 Engine::Components::Vector2::toNative()
{
	return { x, y };
}

Engine::Components::Vector2^ Engine::Components::Vector2::add(Engine::Components::Vector2^ other)
{
	auto v2 = Engine::Components::Vector2::Create();
	v2->copy(this);

	v2->x += other->x;
	v2->y += other->y;

	return v2;
}

Engine::Components::Vector2^ Engine::Components::Vector2::sub(Engine::Components::Vector2^ other)
{
	auto v2 = Engine::Components::Vector2::Create();
	v2->copy(this);

	v2->x -= other->x;
	v2->y -= other->y;

	return v2;
}

Engine::Components::Vector2^ Engine::Components::Vector2::multiply(Engine::Components::Vector2^ other)
{
	auto v2 = Engine::Components::Vector2::Create();
	v2->copy(this);

	v2->x *= other->x;
	v2->y *= other->y;

	return v2;
}

Engine::Components::Vector2^ Engine::Components::Vector2::divide(Engine::Components::Vector2^ other)
{
	auto v2 = Engine::Components::Vector2::Create();
	v2->copy(this);

	v2->x /= other->x;
	v2->y /= other->y;

	return v2;
}

void Engine::Components::Vector2::copy(const Vector2^ origin)
{
	this->x = origin->x;
	this->y = origin->y;
}