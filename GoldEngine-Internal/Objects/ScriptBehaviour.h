#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class ScriptBehaviour : public Engine::Internal::Components::GameObject
	{
	public:
		System::String^ assemblyReference;
		Engine::Scripting::AttributeManager^ attributes;

	public:
		ScriptBehaviour(System::String^ name, Engine::Internal::Components::Transform^ transform) : Engine::Internal::Components::GameObject(name, transform, Engine::Internal::Components::ObjectType::Script, this->tag, Engine::Scripting::LayerManager::GetLayerFromId(1))
		{
			attributes = gcnew Engine::Scripting::AttributeManager();
			assemblyReference = GetType()->FullName->ToString();
		}

		~ScriptBehaviour()
		{
			delete attributes;
			attributes = nullptr;
			assemblyReference = nullptr;
		}

	public:
		void HookUpdate() override
		{
			if (attributes == nullptr) return;

			try
			{
				for each (auto prop in GetType()->GetProperties(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto _attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in _attributes)
					{
						String^ attribName = attrib->attributeName;

						if (attribName == "")
							attribName = prop->Name;

						auto value = this->attributes->getAttribute(attribName)->getValue();
						if (value != nullptr)
						{
							if (prop->PropertyType->IsEnum)
							{
								prop->SetValue(this, (System::Object^)System::Enum::Parse(prop->PropertyType, this->attributes->getAttribute(attribName)->getValue()->ToString()));
							}
							else
							{
								prop->SetValue(this, this->attributes->getAttribute(attribName)->getValue());
							}
						}
						else
						{
							value = prop->GetValue(this);
							if (value != nullptr && value != this->attributes->getAttribute(attribName)->getValue())
							{
								this->attributes->getAttribute(attribName)->setValue(prop->GetValue(this));
							}
						}
					}
				}

				for each (auto prop in GetType()->GetFields(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto _attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in _attributes)
					{
						String^ attribName = attrib->attributeName;

						if (attribName == "")
							attribName = prop->Name;

						auto value = this->attributes->getAttribute(attribName)->getValue();
						if (value != nullptr)
						{
							if (prop->FieldType->IsEnum)
							{
								prop->SetValue(this, (System::Object^)System::Enum::Parse(prop->FieldType, this->attributes->getAttribute(attribName)->getValue()->ToString()));
							}
							else
							{
								prop->SetValue(this, this->attributes->getAttribute(attribName)->getValue());
							}
						}
						else
						{
							value = prop->GetValue(this);
							if (value != nullptr && value != this->attributes->getAttribute(attribName)->getValue())
							{
								this->attributes->getAttribute(attribName)->setValue(prop->GetValue(this));
							}
						}
					}
				}

			}
			catch (Exception^ ex)
			{
				printError(ex->Message);
			}
		}

	public:
		virtual void Init() override
		{
			attributes->DeserializeAttributes();

			try
			{
				for each (auto prop in GetType()->GetProperties(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in attributes)
					{
						String^ attributeName = attrib->attributeName;

						if (attributeName == "")
							attributeName = prop->Name;

						if (!this->attributes->hasAttribute(attributeName))
						{
							auto attribv = Engine::Scripting::Attribute::create(
								attrib->accessLevel,
								attributeName,
								prop->GetValue(this),
								prop->PropertyType
							);

							this->attributes->setAttribute_suppressed(
								attribv
							);

							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->PropertyType);
						}
						else
						{
							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->PropertyType);
						}
					}
				}

				for each (auto prop in GetType()->GetFields(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in attributes)
					{
						String^ attributeName = attrib->attributeName;

						if (attributeName == "")
							attributeName = prop->Name;

						if (!this->attributes->hasAttribute(attributeName))
						{
							auto attribv = Engine::Scripting::Attribute::create(
								attrib->accessLevel,
								attributeName,
								prop->GetValue(this),
								prop->FieldType
							);

							this->attributes->setAttribute_suppressed(
								attribv
							);

							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->FieldType);
						}
						else
						{
							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->FieldType);
						}
					}
				}

				attributes->DeserializeAttributes();
				HookUpdate();
			}
			catch (Exception^ ex)
			{

			}
		}

		virtual void Setup() override
		{
			attributes->DeserializeAttributes();

			try
			{
				for each (auto prop in GetType()->GetProperties(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in attributes)
					{
						String^ attributeName = attrib->attributeName;

						if (attributeName == "")
							attributeName = prop->Name;

						if (!this->attributes->hasAttribute(attributeName))
						{
							auto attribv = Engine::Scripting::Attribute::create(
								attrib->accessLevel,
								attributeName,
								prop->GetValue(this),
								prop->PropertyType
							);

							this->attributes->setAttribute_suppressed(
								attribv
							);

							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->PropertyType);
						}
						else
						{
							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->PropertyType);
						}
					}
				}

				for each (auto prop in GetType()->GetFields(System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::CreateInstance))
				{
					auto attributes = prop->GetCustomAttributes(Engine::Scripting::SerializePropertyAttribute::typeid, true);
					for each (Engine::Scripting::SerializePropertyAttribute ^ attrib in attributes)
					{
						String^ attributeName = attrib->attributeName;

						if (attributeName == "")
							attributeName = prop->Name;

						if (!this->attributes->hasAttribute(attributeName))
						{
							auto attribv = Engine::Scripting::Attribute::create(
								attrib->accessLevel,
								attributeName,
								prop->GetValue(this),
								prop->FieldType
							);

							this->attributes->setAttribute_suppressed(
								attribv
							);

							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->FieldType);
						}
						else
						{
							this->attributes->getAttribute(attributeName)->setPropertyDescriptor(prop, this);
							this->attributes->getAttribute(attributeName)->userDataType = gcnew Engine::Reflectable::ReflectableType(prop->FieldType);
						}
					}
				}

				attributes->DeserializeAttributes();
				HookUpdate();
			}
			catch (Exception^ ex)
			{

			}
		}
	};
}