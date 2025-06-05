#include "../SDK.h"
#include "../Material.h"

Engine::Components::Locs::FloatLoc::FloatLoc(float value)
{
	this->value = value;
}

Engine::Components::Locs::FloatLoc::FloatLoc()
{

}

void Engine::Components::Locs::FloatLoc::SetValue(float value)
{
	this->value = value;
}

unsigned int Engine::Components::Locs::FloatLoc::GetLocType()
{
	return 2;
}