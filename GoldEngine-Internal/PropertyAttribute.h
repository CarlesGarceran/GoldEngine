#pragma once

namespace Engine::Scripting
{
	[System::AttributeUsageAttribute(System::AttributeTargets::Field | System::AttributeTargets::Property)]
    public ref class PropertyAttribute : System::Attribute
    {
    public:
		System::String^ attributeName;

	public:
		PropertyAttribute(System::String^ name)
		{
			this->attributeName = name;
		}

		PropertyAttribute()
		{
			this->attributeName = "";
		}
    };
}