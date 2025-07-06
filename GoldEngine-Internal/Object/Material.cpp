#include "../Instantiable.h"
#include "../SDK.h"
#include "Material.h"

using namespace Engine::Components;
using namespace Engine::Reflectable::Generic;

bool containsLoc(System::String^ locName, System::Collections::Generic::List<Locs::Generic::ShaderLoc^>^ locations)
{
	for each(auto loc in locations)
	{
		if (loc->locName->Equals(locName))
			return true;
	}

	return false;
}

int getLocIndex(System::String^ locName, System::Collections::Generic::List<Locs::Generic::ShaderLoc^>^ locations)
{
	int index = 0;

	for each(auto loc in locations)
	{
		if (loc->locName->Equals(locName))
			break;

		index++;
	}

	return index;
}


cli::array<System::Reflection::FieldInfo^>^ GetFields(System::Type^ type)
{
	return type->GetFields();
}

cli::array<String^>^ GetFieldName(cli::array<System::Reflection::FieldInfo^>^ properties)
{
	List<String^>^ props = gcnew List<String^>();

	for each(System::Reflection::FieldInfo ^ property in properties)
	{
		props->Add(property->Name);
	}

	return props->ToArray();
}

System::Reflection::FieldInfo^ GetField(System::Type^ type, String^ propertyName)
{
	return type->GetField(propertyName);
}

auto GetFieldValue(System::Object^ instance, System::Reflection::FieldInfo^ property)
{
	return property->GetValue(instance);
}

Engine::Components::Material::Material(unsigned int shaderId)
{
	this->shaderId = gcnew Engine::Reflectable::Generic::Reflectable<unsigned int>(shaderId);
	MaterialProperties = gcnew System::Collections::Generic::List<Locs::Generic::ShaderLoc^>();
	
	MaterialProperties->Add(gcnew Locs::Generic::ShaderLoc("baseColor", gcnew Locs::ColorLoc(gcnew Engine::Components::Color(0xFFFFFFFF))));
	MaterialProperties->Add(gcnew Locs::Generic::ShaderLoc("albedoMap", gcnew Locs::TextureLoc(0)));
	MaterialProperties->Add(gcnew Locs::Generic::ShaderLoc("normalMap", gcnew Locs::TextureLoc(0)));
}


[Newtonsoft::Json::JsonConstructorAttribute]
Engine::Components::Material::Material()
{

}

bool Engine::Components::Material::AddProperty(System::String^ propName, Locs::Generic::MaterialLoc^ arg)
{
	if (!containsLoc(propName, MaterialProperties))
	{
		MaterialProperties->Add(gcnew Locs::Generic::ShaderLoc(propName, arg));
		return true;
	}

	return false;
}

bool Engine::Components::Material::UpdateProperty(System::String^ propName, Locs::Generic::MaterialLoc^ arg)
{
	if (containsLoc(propName, MaterialProperties))
	{
		MaterialProperties[getLocIndex(propName, MaterialProperties)]->SetMaterialLocation(arg);
		return true;
	}

	return false;
}

bool Engine::Components::Material::RemoveProperty(System::String^ propName)
{
	if (containsLoc(propName, MaterialProperties))
	{
		MaterialProperties->Remove(MaterialProperties[getLocIndex(propName, MaterialProperties)]);
		return true;
	}

	return false;
}

Locs::Generic::MaterialLoc^ Engine::Components::Material::GetMaterialProperty(System::String^ propName)
{
	if (containsLoc(propName, MaterialProperties))
	{
		return ((MaterialProperties[getLocIndex(propName, MaterialProperties)]))->GetMaterialLocation();
	}

	return nullptr;
}

Locs::Generic::MaterialLoc^ Engine::Components::Material::GetBaseColor()
{
	if (containsLoc("baseColor", MaterialProperties))
	{
		return MaterialProperties[getLocIndex("baseColor", MaterialProperties)]->GetMaterialLocation();
	}

	return nullptr;
}

unsigned int Engine::Components::Material::GetMainTexture()
{
	if (containsLoc("albedoMap", MaterialProperties))
	{
		return ((Locs::TextureLoc^)MaterialProperties[getLocIndex("albedoMap", MaterialProperties)])->textureId;
	}

	return 0;
}

unsigned int Engine::Components::Material::GetNormalMap()
{
	if (containsLoc("normalMap", MaterialProperties))
	{
		return ((Locs::TextureLoc^)MaterialProperties[getLocIndex("normalMap", MaterialProperties)])->textureId;
	}

	return 0;
}

generic <class T>
T Engine::Components::Material::GetMaterialProperty(System::String^ propName)
{
	if (containsLoc(propName, MaterialProperties))
	{
		return (T)(MaterialProperties[getLocIndex(propName, MaterialProperties)]);
	}

	return T();
}

void Engine::Components::Material::SetMainTexture(int textureId)
{
	if (containsLoc("albedoMap", MaterialProperties))
	{
		((Locs::TextureLoc^)MaterialProperties[getLocIndex("albedoMap", MaterialProperties)])->textureId = textureId;
	}
}

void Engine::Components::Material::SetNormalMap(int textureId)
{
	if (containsLoc("normalMap", MaterialProperties))
	{
		((Locs::TextureLoc^)MaterialProperties[getLocIndex("normalMap", MaterialProperties)])->textureId = textureId;
	}
}

void Engine::Components::Material::SerializeProperties()
{
	this->shaderId->serialize();

	for each(auto prop in MaterialProperties)
	{
		prop->serialize();
	}
}

void Engine::Components::Material::DeserializeProperties()
{
	this->shaderId->deserialize();

	for each(auto prop in MaterialProperties)
	{
		prop->deserialize();
	}
}

Engine::Components::Material^ Engine::Components::Material::Create()
{
	return gcnew Engine::Components::Material();
}

Engine::Components::Material^ Engine::Components::Material::New()
{
	return gcnew Engine::Components::Material();
}

void Engine::Components::Material::ApplyToShader(RAYLIB::Shader& shader)
{
	for each (auto key in this->MaterialProperties)
	{
		Engine::Components::Locs::Generic::MaterialLoc^ genericLoc = key->GetMaterialLocation();

		int locType = genericLoc->GetLocType();

		switch (locType)
		{
		case ColorLoc:
		{
			auto array = ((Engine::Components::Locs::ColorLoc^)genericLoc)->color->toFloat();
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), array.data(), RAYLIB::SHADER_ATTRIB_VEC4);
			break;
		}
		case TextureLoc:
		{
			RAYLIB::Texture2D texture2D = DataPacks::singleton().GetTexture2D(((Engine::Components::Locs::TextureLoc^)genericLoc)->textureId);
			SetShaderValueTexture(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), texture2D);
			break;
		}
		case FloatLoc:
		{
			auto value = ((Engine::Components::Locs::FloatLoc^)genericLoc)->value;
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &value, RAYLIB::SHADER_ATTRIB_FLOAT);
			break;
		}
		case StructLoc:
		{
			Engine::Components::Locs::StructLoc^ structLoc = (Engine::Components::Locs::StructLoc^)genericLoc;
			auto managedObject = structLoc->getInstance();

			if (managedObject == nullptr)
				break;

			System::Type^ type = managedObject->GetType();

			if (type == System::Array::typeid || type->IsSubclassOf(System::Array::typeid))
			{
				System::Array^ arrayPtr = (System::Array^)managedObject;

				for (int i = 0; i < arrayPtr->Length; i++)
				{
					System::Object^ entry = arrayPtr->GetValue(i);

					auto properties = GetFields(entry->GetType());
					auto propNames = GetFieldName(properties);

					for each(String ^ propertyName in propNames)
					{
						std::string pName = CastStringToNative(propertyName);
						System::Reflection::FieldInfo^ prop = GetField(entry->GetType(), propertyName);

						if (prop->FieldType == TextureId::typeid)
						{
							unsigned int value = ((TextureId)GetFieldValue(entry, prop)).textureId;
							SetShaderValueTexture(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), DataPacks::singleton().GetTexture2D(value));
							continue;
						}

						if (prop->FieldType == int::typeid)
						{
							int value = (int)GetFieldValue(entry, prop);
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_INT);
							continue;
						}

						if (prop->FieldType == float::typeid)
						{
							float value = (float)GetFieldValue(entry, prop);
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_FLOAT);
							continue;
						}

						if (prop->FieldType == Engine::Components::Vector2::typeid)
						{
							RAYLIB::Vector2 value = ((Engine::Components::Vector2^)GetFieldValue(entry, prop))->toNative();
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_VEC2);
							continue;
						}

						if (prop->FieldType == Engine::Components::Vector3::typeid)
						{
							RAYLIB::Vector3 value = ((Engine::Components::Vector3^)GetFieldValue(entry, prop))->toNative();
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_VEC3);
							continue;
						}

						if (prop->FieldType == Engine::Components::Quaternion::typeid)
						{
							RAYLIB::Vector4 value = ((Engine::Components::Quaternion^)GetFieldValue(entry, prop))->toNative();
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_VEC4);
							continue;
						}

						if (prop->FieldType == Engine::Components::Color::typeid)
						{
							auto value = ((Engine::Components::Color^)GetFieldValue(entry, prop))->toFloat();
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), value.data(), SHADER_UNIFORM_VEC4);
							continue;
						}

						if (prop->FieldType == bool::typeid)
						{
							int value = (int)((bool)GetFieldValue(entry, prop));
							SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "[" + std::to_string(i) + "]." + pName).c_str()), &value, SHADER_UNIFORM_INT);
							continue;
						}
					}
				}
			}
			else
			{
				auto properties = GetFields(managedObject->GetType());
				auto propNames = GetFieldName(properties);

				for each(String^ propertyName in propNames)
				{
					std::string pName = CastStringToNative(propertyName);
					System::Reflection::FieldInfo^ prop = GetField(managedObject->GetType(), propertyName);
					
					if (prop->FieldType == TextureId::typeid)
					{
						unsigned int value = ((TextureId)GetFieldValue(managedObject, prop)).textureId;
						SetShaderValueTexture(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), DataPacks::singleton().GetTexture2D(value));
						continue;
					}

					if (prop->FieldType == int::typeid)
					{
						int value = (int)GetFieldValue(managedObject, prop);
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_INT);
						continue;
					}
					
					if (prop->FieldType == float::typeid)
					{
						float value = (float)GetFieldValue(managedObject, prop);
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_FLOAT);
						continue;
					}

					if (prop->FieldType == Engine::Components::Vector2::typeid)
					{
						RAYLIB::Vector2 value = ((Engine::Components::Vector2^)GetFieldValue(managedObject, prop))->toNative();
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_VEC2);
						continue;
					}

					if (prop->FieldType == Engine::Components::Vector3::typeid)
					{
						RAYLIB::Vector3 value = ((Engine::Components::Vector3^)GetFieldValue(managedObject, prop))->toNative();
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_VEC3);
						continue;
					}

					if (prop->FieldType == Engine::Components::Quaternion::typeid)
					{
						RAYLIB::Vector4 value = ((Engine::Components::Quaternion^)GetFieldValue(managedObject, prop))->toNative();
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_VEC4);
						continue;
					}

					if (prop->FieldType == Engine::Components::Color::typeid)
					{
						auto value = ((Engine::Components::Color^)GetFieldValue(managedObject, prop))->toFloat();
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), value.data(), SHADER_UNIFORM_VEC4);
						continue;
					}

					if (prop->FieldType == bool::typeid)
					{
						int value = (int)((bool)GetFieldValue(managedObject, prop));
						SetShaderValue(shader, GetShaderLocation(shader, (CastStringToNative(key->locName) + "." + pName).c_str()), &value, SHADER_UNIFORM_INT);
						continue;
					}
				}
			}
			break;
		}
		case Vector2Loc:
		{
			auto vec2 = ((Engine::Components::Locs::Vector2Loc^)genericLoc)->value;
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &(vec2->toNative()), RAYLIB::SHADER_ATTRIB_VEC2);
			break;
		}
		}
	}
}
