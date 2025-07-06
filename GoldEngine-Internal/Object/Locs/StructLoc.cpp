#include "../SDK.h"
#include "../Material.h"
#include "StructLoc.h"

Engine::Components::Locs::StructLoc::StructLoc(System::Object^ structPointer)
{
	this->structure = gcnew Engine::Reflectable::Generic::Reflectable<System::Object^>(structPointer);
}

Engine::Components::Locs::StructLoc::StructLoc()
{
	if(this->structure != nullptr)
		this->structure->deserialize();
}

System::Object^ Engine::Components::Locs::StructLoc::getInstance()
{
	return this->structure->getInstance();
}

void Engine::Components::Locs::StructLoc::setInstance(System::Object^% instance)
{
	this->structure->setInstance(instance);
}

unsigned int Engine::Components::Locs::StructLoc::GetLocType()
{
	return 3;
}