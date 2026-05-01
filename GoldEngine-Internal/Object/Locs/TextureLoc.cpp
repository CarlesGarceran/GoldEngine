#include "../SDK.h"
#include "../Material.h"
#include "TextureLoc.h"

Engine::Components::Locs::TextureLoc::TextureLoc(unsigned int texId)
{
	this->textureId = texId;
}

Engine::Components::Locs::TextureLoc::TextureLoc()
{

}

unsigned int Engine::Components::Locs::TextureLoc::GetLocType()
{
	return (unsigned int)Engine::Components::Enums::MaterialLocations::TextureLoc;
}

System::Object^ Engine::Components::Locs::TextureLoc::GetValue()
{
	return this->textureId;
}

void Engine::Components::Locs::TextureLoc::SetValue(System::Object^ instance)
{
	this->textureId = (unsigned int)System::Convert::ToUInt32(instance);
}
