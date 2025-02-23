#include "../Reflection/ReflectedType.h"
#include "../Reflection/ReflectableType.h"
#include "../SDK.h"
#include "../Material.h"
#include "TextureLoc.h"

Engine::Components::Locs::TextureLoc::TextureLoc(unsigned int texId)
{
	textureId = gcnew Engine::Reflectable::Generic::Reflectable<unsigned int>(texId);
}

void Engine::Components::Locs::TextureLoc::operator[](unsigned int textureId)
{
	this->textureId->Instance = textureId;
}

void Engine::Components::Locs::TextureLoc::operator=(unsigned int textureId)
{
	this->textureId->Instance = textureId;
}

unsigned int Engine::Components::Locs::TextureLoc::GetLocType()
{
	return 1;
}