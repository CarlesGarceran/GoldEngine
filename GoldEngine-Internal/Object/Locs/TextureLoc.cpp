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