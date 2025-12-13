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

System::Object^ Engine::Components::Locs::Vector3Loc::GetValue()
{
	return this->value;
}

void Engine::Components::Locs::Vector3Loc::SetValue(System::Object^ inst)
{
	if (inst->GetType() != Engine::Components::Vector3::typeid) throw gcnew System::ArgumentException();

	this->value = (Engine::Components::Vector3)inst;
}
