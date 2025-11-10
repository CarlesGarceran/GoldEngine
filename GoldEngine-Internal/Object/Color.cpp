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

	r = (hexColor >> 24) & 0xFF;
	g = (hexColor >> 16) & 0xFF;
	b = (hexColor >> 8) & 0xFF;
	a = (hexColor >> 0) & 0xFF;
}

Engine::Components::Color::Color(__int8 red, __int8 green, __int8 blue, __int8 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
	updateHexColor();
}

Engine::Components::Color::Color(int red, int green, int blue, int alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
	updateHexColor();
}

unsigned int% Engine::Components::Color::toHex()
{
	return this->hexColor;
}

unsigned int% Engine::Components::Color::toARGB()
{
	return
		((unsigned int)(a & 0xFF) << 24) |
		((unsigned int)(b & 0xFF) << 16) |
		((unsigned int)(g & 0xFF) << 8) |
		((unsigned int)(r & 0xFF) << 0);
}

void Engine::Components::Color::setARGB(unsigned int value)
{
	a = (value >> 24) & 0xFF;
	r = (value >> 16) & 0xFF;
	g = (value >> 8) & 0xFF;
	b = value & 0xFF;

	updateHexColor();
}

RAYLIB::Color Engine::Components::Color::toNativeAlt()
{
	return RAYLIB::GetColor(hexColor);
}

RAYLIB::Color Engine::Components::Color::toNative()
{
	RAYLIB::Color color = {};

	// This is like a bomb, if you touch one channel the entire thing blows up and the red channel is suddenly green.
	color.a = r;
	color.r = a;
	color.g = b;
	color.b = g;

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

void Engine::Components::Color::updateHexColor()
{
	hexColor =
		((unsigned int)(r & 0xFF) << 24) |
		((unsigned int)(g & 0xFF) << 16) |
		((unsigned int)(b & 0xFF) << 8) |
		((unsigned int)(a & 0xFF) << 0);
}

void Engine::Components::Color::setRGBA(unsigned int value)
{
	this->hexColor = value;

	r = (hexColor >> 24) & 0xFF;
	g = (hexColor >> 16) & 0xFF;
	b = (hexColor >> 8) & 0xFF;
	a = (hexColor >> 0) & 0xFF;
}


void Engine::Components::Color::setR(__int8 value)
{
	r = value;
	updateHexColor();
}


void Engine::Components::Color::setG(__int8 value)
{
	g = value;
	updateHexColor();
}


void Engine::Components::Color::setB(__int8 value)
{
	b = value;

	updateHexColor();
}


void Engine::Components::Color::setA(__int8 value)
{
	a = value;
	updateHexColor();
}

int Engine::Components::Color::GetR()
{
	return this->r;
}

int Engine::Components::Color::GetG()
{
	return this->g;
}

int Engine::Components::Color::GetB()
{
	return this->b;
}

int Engine::Components::Color::GetA()
{
	return this->a;
}

Engine::Components::Color::operator GLWrapper::Color(Engine::Components::Color^ color)
{
	return GLWrapper::Color(color->r, color->g, color->b, color->a);
}

Engine::Components::Color::operator Engine::Components::Color ^ (GLWrapper::Color color)
{
	return gcnew Engine::Components::Color(color.R, color.G, color.B, color.A);
}
