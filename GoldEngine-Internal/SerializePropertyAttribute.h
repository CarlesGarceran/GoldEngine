#pragma once

namespace Engine::Scripting
{
	public ref class SerializePropertyAttribute : System::Attribute
	{
	public:
		Engine::Scripting::AccessLevel accessLevel;
		String^ attributeName;

	public:
		SerializePropertyAttribute(Engine::Scripting::AccessLevel level, String^ name)
		{
			this->accessLevel = level;
			this->attributeName = name;
		}

		SerializePropertyAttribute(Engine::Scripting::AccessLevel level)
		{
			this->accessLevel = level;
			this->attributeName = "";
		}

		SerializePropertyAttribute()
		{
			this->accessLevel = Engine::Scripting::AccessLevel::Public;
			this->attributeName = "";
		}
	};
}