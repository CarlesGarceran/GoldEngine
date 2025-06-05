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