#pragma once

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class AttributeManager
	{
	public:
		List<Attribute^>^ attributes;

	public:
		AttributeManager()
		{
			attributes = gcnew List<Attribute^>();
		}

		~AttributeManager()
		{
			for each (Attribute^ attrib in attributes)
			{
				delete attrib;
			}

			attributes->Clear();
		}

		bool hasAttribute(String^ attribute)
		{
			if (this == nullptr) return false;

			for each (Attribute ^ attrib in attributes)
			{
				if (attrib != nullptr && attrib->name == attribute)
					return true;
			}

			return false;
		}

	private:
		Attribute^ GetAttribute(String^ attribute)
		{
			if (this == nullptr) return nullptr;

			for each (Attribute ^ attrib in attributes)
			{
				if (attrib != nullptr && attrib->name == attribute)
					return attrib;
			}

			return nullptr;
		}


	public:
		void DeserializeAttributes()
		{
			if (this == nullptr) return;

			for each (auto x in attributes)
			{
				x->setValueForce(x->DeserializeAttribute(), true);
				x->resetType();
			}
		}
		
		void removeAttribute(Attribute^ attrib)
		{
			if (this == nullptr) return;

			attributes->Remove(attrib);
		}

		void removeAttribute(String^ name)
		{
			if (this == nullptr) return;

			if (hasAttribute(name))
				attributes->Remove(GetAttribute(name));
		}

		void clearAttributes()
		{
			if (this == nullptr) return;

			attributes->Clear();
		}

		void addAttribute(Attribute^ attribute) 
		{
			if (this == nullptr) return;

			setAttribute(attribute);
		}

		void addAttribute(String^ name, System::Object^ data)
		{
			setAttribute(name, data);
		}

		void addAttribute(AccessLevel level, String^ name, System::Object^ data)
		{
			setAttribute(level, name, data);
		}

		void setAttribute(String^ name, System::Object^ data)
		{
			if (!hasAttribute(name))
			{
				attributes->Add(Attribute::New(AccessLevel::Public, name, data));
			}
			else
			{
				printError("Attribute name is duplicate");
			}
		}

		void setAttribute(AccessLevel level, String^ name, System::Object^ data)
		{
			if (!hasAttribute(name))
			{
				attributes->Add(Attribute::New(level, name, data));
			}
			else
			{
				printError("Attribute name is duplicate");
			}
		}

		void setAttribute(Attribute^ newAttrib)
		{
			if (newAttrib == nullptr)
				return;

			if (!hasAttribute(newAttrib->name))
			{
				attributes->Add(newAttrib);
			}
			else
			{
				printError("Attribute name is duplicate");
			}
		}


		void setAttribute_suppressed(Attribute^ newAttrib) // this one doesnt trigger duplicate attrib names output, perfect for silent operations
		{
			if (newAttrib == nullptr)
				return;

			if (!hasAttribute(newAttrib->name))
			{
				attributes->Add(newAttrib);
			}
		}

		Attribute^ getAttribute(String^ name)
		{
			if (hasAttribute(name))
			{
				return GetAttribute(name);
			}
			else
			{
				return nullptr;
			}
		}
	};
}