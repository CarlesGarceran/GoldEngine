#pragma once

namespace Engine::Scripting
{
	[System::AttributeUsageAttribute(System::AttributeTargets::Class)]
	public ref class PersistantInstanceAttribute : System::Attribute
	{
	public:
		PersistantInstanceAttribute() { }
	};
}