#include "../SDK.h"
#include "../Material.h"
#include "Vector2Loc.h"

Engine::Components::Locs::Vector2Loc::Vector2Loc(Engine::Components::Vector2^ value)
{
    this->value = value;
}

Engine::Components::Locs::Vector2Loc::Vector2Loc()
{

}

unsigned int Engine::Components::Locs::Vector2Loc::GetLocType()
{
    return 4;
}
