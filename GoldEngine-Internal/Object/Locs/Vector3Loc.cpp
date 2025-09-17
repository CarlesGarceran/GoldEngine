#include "../SDK.h"
#include "../Material.h"
#include "Vector3Loc.h"

Engine::Components::Locs::Vector3Loc::Vector3Loc(Engine::Components::Vector3 value)
{
	this->value = value;
}

Engine::Components::Locs::Vector3Loc::Vector3Loc()
{

}

unsigned int Engine::Components::Locs::Vector3Loc::GetLocType()
{
	return 5;
}
