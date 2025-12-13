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
	return 1;
}

System::Object^ Engine::Components::Locs::TextureLoc::GetValue()
{
	return this->textureId;
}

void Engine::Components::Locs::TextureLoc::SetValue(System::Object^ instance)
{
	//if (instance->GetType() != System::UInt32::typeid) throw gcnew System::ArgumentException();

	this->textureId = (unsigned int)System::Convert::ToUInt32(instance);
}
