#include "../Instantiable.h"
#include "../SDK.h"
#include "Material.h"

using namespace Engine::Components;
using namespace Engine::Components::Enums;
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
		return ((Locs::TextureLoc^)MaterialProperties[getLocIndex("albedoMap", MaterialProperties)]->GetMaterialLocation())->textureId;
	}

	return 0;
}

unsigned int Engine::Components::Material::GetNormalMap()
{
	if (containsLoc("normalMap", MaterialProperties))
	{
		return ((Locs::TextureLoc^)MaterialProperties[getLocIndex("normalMap", MaterialProperties)]->GetMaterialLocation())->textureId;
	}

	return 0;
}

generic <class T>
T Engine::Components::Material::GetMaterialProperty(System::String^ propName)
{
	if (containsLoc(propName, MaterialProperties))
	{
		return (T)(MaterialProperties[getLocIndex(propName, MaterialProperties)]->GetMaterialLocation());
	}

	return T();
}

void Engine::Components::Material::SetMainTexture(int textureId)
{
	if (containsLoc("albedoMap", MaterialProperties))
	{
		int locIndex = getLocIndex("albedoMap", MaterialProperties);
		auto matLoc = ((Locs::TextureLoc^)MaterialProperties[locIndex]->GetMaterialLocation());
		matLoc->textureId = textureId;

		MaterialProperties[locIndex]->SetMaterialLocation(matLoc);
	}
}

void Engine::Components::Material::SetNormalMap(int textureId)
{
	if (containsLoc("normalMap", MaterialProperties))
	{
		int locIndex = getLocIndex("normalMap", MaterialProperties);
		auto matLoc = ((Locs::TextureLoc^)MaterialProperties[locIndex]->GetMaterialLocation());
		matLoc->textureId = textureId;

		MaterialProperties[locIndex]->SetMaterialLocation(matLoc);
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

void DoTextureLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key, Engine::Components::Locs::Generic::MaterialLoc^ genericLoc)
{
	RAYLIB::Texture2D% texture2D = DataPacks::singleton().GetTexture2D((unsigned int)genericLoc->GetValue());
	SetShaderValueTexture(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), texture2D);
}

void DoFloatLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key, Engine::Components::Locs::Generic::MaterialLoc^ genericLoc)
{
	auto value = (float)genericLoc->GetValue();
	SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &value, RAYLIB::SHADER_ATTRIB_FLOAT);
}

void DoColorLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key, Engine::Components::Locs::Generic::MaterialLoc^ genericLoc)
{
	auto array = ((Engine::Components::Color^)genericLoc->GetValue())->toFloat();
	SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), array.data(), RAYLIB::SHADER_ATTRIB_VEC4);
}

void ResetTextureLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key)
{
	SetShaderValueTexture(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), DataPacks::singleton().GetFallbackTexture());
}

void ResetFloatLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key)
{
	auto value = 0.0f;
	SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &value, RAYLIB::SHADER_ATTRIB_FLOAT);
}

void ResetColorLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key)
{
	float value = 0xFFFFFFFF;
	SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &value, RAYLIB::SHADER_ATTRIB_VEC4);
}

void DoStructLoc(RAYLIB::Shader& shader, Locs::Generic::ShaderLoc^ key, Engine::Components::Locs::Generic::MaterialLoc^ genericLoc)
{
	auto managedObject = genericLoc->GetValue();

	if (managedObject == nullptr)
		return;

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
				System::Reflection::FieldInfo^ prop = GetField(entry->GetType(), propertyName);
				std::string location = (CastStringToNative(key->locName + "[" + i.ToString() + "]." + propertyName));

				if (prop->FieldType == Engine::Components::Matrix16::typeid)
				{
					Engine::Components::Matrix16^ matrix = (Engine::Components::Matrix16^)GetFieldValue(entry, prop);
					SetShaderValueMatrix(shader, GetShaderLocation(shader, location.c_str()), matrix->toNative());
					continue;
				}

				if (prop->FieldType == GLWrapper::Texture2D::typeid)
				{
					GLWrapper::Texture2D managedTex = (GLWrapper::Texture2D)GetFieldValue(entry, prop);
					RAYLIB::Texture2D nativeTex = { 0 };
					nativeTex.id = managedTex.Id;
					nativeTex.width = managedTex.Width;
					nativeTex.height = managedTex.Height;
					nativeTex.mipmaps = managedTex.Mipmaps;
					nativeTex.format = (int)managedTex.Format;

					SetShaderValueTexture(shader, GetShaderLocation(shader, location.c_str()), nativeTex);
					continue;
				}


				if (prop->FieldType == TextureId::typeid)
				{
					unsigned int value = ((TextureId)GetFieldValue(entry, prop)).textureId;
					SetShaderValueTexture(shader, GetShaderLocation(shader, location.c_str()), DataPacks::singleton().GetTexture2D(value));
					continue;
				}

				if (prop->FieldType == int::typeid)
				{
					int value = (int)GetFieldValue(entry, prop);
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_INT);
					continue;
				}

				if (prop->FieldType == float::typeid)
				{
					float value = (float)GetFieldValue(entry, prop);
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_FLOAT);
					continue;
				}

				if (prop->FieldType == Engine::Components::Vector2::typeid)
				{
					RAYLIB::Vector2 value = ((Engine::Components::Vector2)GetFieldValue(entry, prop)).toNative();
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC2);
					continue;
				}

				if (prop->FieldType == Engine::Components::Vector3::typeid)
				{
					RAYLIB::Vector3 value = ((Engine::Components::Vector3)GetFieldValue(entry, prop)).toNative();
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC3);
					continue;
				}

				if (prop->FieldType == Engine::Components::Quaternion::typeid)
				{
					RAYLIB::Vector4 value = ((Engine::Components::Quaternion)GetFieldValue(entry, prop)).toNative();
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC4);
					continue;
				}

				if (prop->FieldType == Engine::Components::Color::typeid)
				{
					auto value = ((Engine::Components::Color^)GetFieldValue(entry, prop))->toFloat();
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), value.data(), SHADER_UNIFORM_VEC4);
					continue;
				}

				if (prop->FieldType == bool::typeid)
				{
					int value = (int)((bool)GetFieldValue(entry, prop));
					SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_INT);
					continue;
				}
			}
		}
	}
	else
	{
		auto properties = GetFields(managedObject->GetType());
		auto propNames = GetFieldName(properties);

		for each(String ^ propertyName in propNames)
		{
			System::Reflection::FieldInfo^ prop = GetField(managedObject->GetType(), propertyName);
			auto entry = managedObject;

			std::string location = CastStringToNative(key->locName + "." + propertyName);

			if (prop->FieldType == Engine::Components::Matrix16::typeid)
			{
				Engine::Components::Matrix16^ matrix = (Engine::Components::Matrix16^)GetFieldValue(entry, prop);
				SetShaderValueMatrix(shader, GetShaderLocation(shader, location.c_str()), matrix->toNative());
				continue;
			}

			if (prop->FieldType == GLWrapper::Texture2D::typeid)
			{
				GLWrapper::Texture2D managedTex = (GLWrapper::Texture2D)GetFieldValue(entry, prop);
				RAYLIB::Texture2D nativeTex = { 0 };
				nativeTex.id = managedTex.Id;
				nativeTex.width = managedTex.Width;
				nativeTex.height = managedTex.Height;
				nativeTex.mipmaps = managedTex.Mipmaps;
				nativeTex.format = (int)managedTex.Format;
				
				SetShaderValueTexture(shader, GetShaderLocation(shader, location.c_str()), nativeTex);
				continue;
			}
			
			if (prop->FieldType == TextureId::typeid)
			{
				unsigned int value = ((TextureId)GetFieldValue(entry, prop)).textureId;
				SetShaderValueTexture(shader, GetShaderLocation(shader, location.c_str()), DataPacks::singleton().GetTexture2D(value));
				continue;
			}

			if (prop->FieldType == int::typeid)
			{
				int value = (int)GetFieldValue(entry, prop);
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_INT);
				continue;
			}

			if (prop->FieldType == float::typeid)
			{
				float value = (float)GetFieldValue(entry, prop);
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_FLOAT);
				continue;
			}

			if (prop->FieldType == Engine::Components::Vector2::typeid)
			{
				RAYLIB::Vector2 value = ((Engine::Components::Vector2)GetFieldValue(entry, prop)).toNative();
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC2);
				continue;
			}

			if (prop->FieldType == Engine::Components::Vector3::typeid)
			{
				RAYLIB::Vector3 value = ((Engine::Components::Vector3)GetFieldValue(entry, prop)).toNative();
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC3);
				continue;
			}

			if (prop->FieldType == Engine::Components::Quaternion::typeid)
			{
				RAYLIB::Vector4 value = ((Engine::Components::Quaternion)GetFieldValue(entry, prop)).toNative();
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_VEC4);
				continue;
			}

			if (prop->FieldType == Engine::Components::Color::typeid)
			{
				auto value = ((Engine::Components::Color^)GetFieldValue(entry, prop))->toFloat();
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), value.data(), SHADER_UNIFORM_VEC4);
				continue;
			}

			if (prop->FieldType == bool::typeid)
			{
				int value = (int)((bool)GetFieldValue(entry, prop));
				SetShaderValue(shader, GetShaderLocation(shader, location.c_str()), &value, SHADER_UNIFORM_INT);
				continue;
			}
		}
	}
}

void Engine::Components::Material::ApplyToShader(RAYLIB::Shader& shader)
{
	RLGL::rlEnableShader(shader.id);

	for each (auto key in this->MaterialProperties)
	{
		Engine::Components::Locs::Generic::MaterialLoc^ genericLoc = key->GetMaterialLocation();

		int locType = genericLoc->GetLocType();

		switch (locType)
		{
		case ColorLoc:
		{
			DoColorLoc(shader, key, genericLoc);
			break;
		}
		case TextureLoc:
		{
			DoTextureLoc(shader, key, genericLoc);
			break;
		}
		case FloatLoc:
		{
			DoFloatLoc(shader, key, genericLoc);
			break;
		}
		case StructLoc:
		{
			DoStructLoc(shader, key, genericLoc);
			break;
		}
		case Vector2Loc:
		{
			auto vec2 = (Engine::Components::Vector2)genericLoc->GetValue();
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &(vec2.toNative()), RAYLIB::SHADER_ATTRIB_VEC2);
			break;
		}
		}
	}
}

void Engine::Components::Material::ResetShader(RAYLIB::Shader& shader)
{
	RLGL::rlReloadTextureUnits();

	for each (auto key in this->MaterialProperties)
	{
		Engine::Components::Locs::Generic::MaterialLoc^ genericLoc = key->GetMaterialLocation();

		int locType = genericLoc->GetLocType();

		switch (locType)
		{
		case ColorLoc:
		{
			ResetColorLoc(shader, key);
			break;
		}
		case TextureLoc:
		{
			ResetTextureLoc(shader, key);
			break;
		}
		case FloatLoc:
		{
			ResetFloatLoc(shader, key);
			break;
		}
		/*
		case StructLoc:
		{
			DoStructLoc(shader, key, genericLoc);
			break;
		}
		*/
		case Vector2Loc:
		{
			RAYLIB::Vector2 vec2 = { 0, 0 };
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &vec2, RAYLIB::SHADER_ATTRIB_VEC2);
			break;
		}
		}
	}
}
