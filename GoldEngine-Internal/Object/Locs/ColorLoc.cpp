#include "../Material.h"
#include "../SDK.h"

Engine::Components::Locs::ColorLoc::ColorLoc(Engine::Components::Color^ color)
{
	this->color = gcnew Engine::Reflectable::Generic::Reflectable<Engine::Components::Color^>(color);
}

void Engine::Components::Locs::ColorLoc::SetColor(Engine::Components::Color^ color)
{
	this->color[color];
}

unsigned int Engine::Components::Locs::ColorLoc::GetLocType()
{
	return 0;
}