#include "../SDK.h"
#include "../Material.h"

Engine::Components::Locs::ColorLoc::ColorLoc(Engine::Components::Color^ color)
{
	this->color = color;
}

Engine::Components::Locs::ColorLoc::ColorLoc()
{

}

void Engine::Components::Locs::ColorLoc::SetColor(Engine::Components::Color^ color)
{
	this->color = color;
}

unsigned int Engine::Components::Locs::ColorLoc::GetLocType()
{
	return 0;
}