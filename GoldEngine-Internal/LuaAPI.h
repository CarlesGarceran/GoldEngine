#pragma once

namespace Engine::Attributes
{
	[System::AttributeUsageAttribute(System::AttributeTargets::Class)]
	public ref class LuaAPIAttribute : System::Attribute
	{
	public:
		System::String^ globalName;
		bool isStatic = true;

		LuaAPIAttribute(System::String^ GlobalId, bool _static)
		{
			this->globalName = GlobalId;
			this->isStatic = _static;
		}

		LuaAPIAttribute(System::String^ GlobalId)
		{
			this->globalName = GlobalId;
			this->isStatic = true;
		}

		LuaAPIAttribute()
		{
			this->globalName = "";
			this->isStatic = true;
		}
	};
}