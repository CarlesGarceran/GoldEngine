#include "../SDK.h"
#include "../Material.h"
#include "Vector2Loc.h"

Engine::Components::Locs::Vector2Loc::Vector2Loc(Engine::Components::Vector2 value)
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

System::Object^ Engine::Components::Locs::Vector2Loc::GetValue()
{
    return this->value;
}

void Engine::Components::Locs::Vector2Loc::SetValue(System::Object^ inst)
{
    if (inst->GetType() != Engine::Components::Vector2::typeid) throw gcnew System::ArgumentException();

    this->value = (Engine::Components::Vector2)inst;
}
