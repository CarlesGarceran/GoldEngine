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
	_r = (colorHex >> 24) & 0xFF;
	_g = (colorHex >> 16) & 0xFF;
	_b = (colorHex >> 8) & 0xFF;
	_a = (colorHex >> 0) & 0xFF;
}

Engine::Components::Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	_r = red;
	_g = green;
	_b = blue;
	_a = alpha;
}

Engine::Components::Color::Color(int red, int green, int blue, int alpha)
{
	_r = red;
	_g = green;
	_b = blue;
	_a = alpha;
}

unsigned int Engine::Components::Color::toHex()
{
	return this->toRGBA();
}

unsigned int Engine::Components::Color::toRGBA()
{
	return
		((unsigned int)(a & 0xFF) << 0) |
		((unsigned int)(b & 0xFF) << 8) |
		((unsigned int)(g & 0xFF) << 16) |
		((unsigned int)(r & 0xFF) << 24);
}

unsigned int Engine::Components::Color::toARGB()
{
	return
		((unsigned int)(a & 0xFF) << 24) |
		((unsigned int)(r & 0xFF) << 16) |
		((unsigned int)(g & 0xFF) << 8) |
		((unsigned int)(b & 0xFF) << 0);
}

void Engine::Components::Color::setARGB(unsigned int value)
{
	a = (value >> 24) & 0xFF;
	r = (value >> 16) & 0xFF;
	g = (value >> 8) & 0xFF;
	b = value & 0xFF;
}

RAYLIB::Color Engine::Components::Color::toNativeAlt()
{
	return RAYLIB::GetColor(ToRGBA());
}

RAYLIB::Color Engine::Components::Color::toNative()
{
	if (this == nullptr) return RAYLIB::BLANK;

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


void Engine::Components::Color::setRGBA(unsigned int value)
{
	_r = (value >> 24) & 0xFF;
	_g = (value >> 16) & 0xFF;
	_b = (value >> 8) & 0xFF;
	_a = (value >> 0) & 0xFF;
}


void Engine::Components::Color::setR(unsigned char value)
{
	_r = value;
}


void Engine::Components::Color::setG(unsigned char value)
{
	_g = value;
}


void Engine::Components::Color::setB(unsigned char value)
{
    _b = value;
}

void Engine::Components::Color::setA(unsigned char value)
{
	_a = value;
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
	RAYLIB::Color _color = color->toNative();

	return GLWrapper::Color(_color.r, _color.g, _color.b, _color.a);
}

Engine::Components::Color::operator Engine::Components::Color ^ (GLWrapper::Color color)
{
	return gcnew Engine::Components::Color(color.R, color.G, color.B, color.A);
}

unsigned char Engine::Components::Color::a::get()
{
	return _a;
}

void Engine::Components::Color::a::set(unsigned char value)
{
	SetA(value);
}

unsigned char Engine::Components::Color::r::get()
{
	return _r;
}

void Engine::Components::Color::r::set(unsigned char value)
{
	SetR(value);
}

unsigned char Engine::Components::Color::g::get()
{
	return _g;
}

void Engine::Components::Color::g::set(unsigned char value)
{
	SetG(value);
}

unsigned char Engine::Components::Color::b::get()
{
	return _b;
}

void Engine::Components::Color::b::set(unsigned char value)
{
	SetB(value);
}
