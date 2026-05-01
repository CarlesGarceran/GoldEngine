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
	return (unsigned int)Engine::Components::Enums::MaterialLocations::ColorLoc;
}

System::Object^ Engine::Components::Locs::ColorLoc::GetValue()
{
	return this->color;
}

void Engine::Components::Locs::ColorLoc::SetValue(System::Object^ instance)
{
	if (instance->GetType() != Engine::Components::Color::typeid) throw gcnew System::ArgumentException();

	this->SetColor((Engine::Components::Color^)instance);
}
