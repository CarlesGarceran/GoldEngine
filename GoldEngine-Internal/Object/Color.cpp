#include "../Includes.h"
#include "../GlIncludes.h"
#include "../ManagedIncludes.h"
#include "Color.h"

using namespace System;
using namespace Engine::Components;

Engine::Components::Color::Color()
{
	// FOR NEWTONSOFT REFLECTION
}

Engine::Components::Color::Color(unsigned int colorHex)
{
	this->hexColor = colorHex;

	r = (hexColor >> 0) & 0xFF;
	g = (hexColor >> 8) & 0xFF;
	b = (hexColor >> 16) & 0xFF;
	a = (hexColor >> 24) & 0xFF;
}

Engine::Components::Color::Color(__int8 red, __int8 green, __int8 blue, __int8 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;

	hexColor = 0;
	hexColor =
		((unsigned int)(a & 0xFF) << 24) |
		((unsigned int)(b & 0xFF) << 16) |
		((unsigned int)(g & 0xFF) << 8) |
		((unsigned int)(r & 0xFF) << 0);
}

Engine::Components::Color::Color(int red, int green, int blue, int alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;

	hexColor = 0;
	hexColor =
		((unsigned int)(a & 0xFF) << 24) |
		((unsigned int)(b & 0xFF) << 16) |
		((unsigned int)(g & 0xFF) << 8) |
		((unsigned int)(r & 0xFF) << 0);
}

unsigned int% Engine::Components::Color::toHex()
{
	return this->hexColor;
}

RAYLIB::Color Engine::Components::Color::toNativeAlt()
{
	return RAYLIB::GetColor(hexColor);
}

RAYLIB::Color Engine::Components::Color::toNative()
{
	__int8 rByte;
	__int8 gByte;
	__int8 bByte;
	__int8 aByte;

	rByte = (hexColor >> 0) & 0xFF;
	gByte = (hexColor >> 8) & 0xFF;
	bByte = (hexColor >> 16) & 0xFF;
	aByte = (hexColor >> 24) & 0xFF;

	RAYLIB::Color color = {};

	color.r = rByte;
	color.g = gByte;
	color.b = bByte;
	color.a = aByte;

	return color;
}

RAYLIB::Vector3 Engine::Components::Color::toNativeVector3()
{
	RAYLIB::Color c = toNative();
	RAYLIB::Vector3 vector = { 0 };

	vector.x = (c.r);
	vector.y = (c.g);
	vector.z = (c.b);

	return vector;
}

std::array<float, 4> Engine::Components::Color::toFloat()
{
	auto native = this->toNative();
	auto normalized = RAYLIB::ColorNormalize(native);

	std::array<float, 4> out = {
		   normalized.x,
		   normalized.y,
		   normalized.z,
		   normalized.w
	};

	return out;
}

void Engine::Components::Color::setHex(unsigned int value)
{
	this->hexColor = value;

	r = (hexColor >> 0) & 0xFF;
	g = (hexColor >> 8) & 0xFF;
	b = (hexColor >> 16) & 0xFF;
	a = (hexColor >> 24) & 0xFF;
}


void Engine::Components::Color::setR(__int8 value)
{
	r = value;

	hexColor = 0;
	hexColor << (r) & 0xFF;
	hexColor << (g) & 0xFF;
	hexColor << (b) & 0xFF;
	hexColor << (a) & 0xFF;
}


void Engine::Components::Color::setG(__int8 value)
{
	g = value;

	hexColor = 0;
	hexColor << (r) & 0xFF;
	hexColor << (g) & 0xFF;
	hexColor << (b) & 0xFF;
	hexColor << (a) & 0xFF;
}


void Engine::Components::Color::setB(__int8 value)
{
	b = value;

	hexColor = 0;
	hexColor << (r) & 0xFF;
	hexColor << (g) & 0xFF;
	hexColor << (b) & 0xFF;
	hexColor << (a) & 0xFF;
}


void Engine::Components::Color::setA(__int8 value)
{
	a = value;

	hexColor = 0;
	hexColor << (r) & 0xFF;
	hexColor << (g) & 0xFF;
	hexColor << (b) & 0xFF;
	hexColor << (a) & 0xFF;
}
