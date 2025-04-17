#include "../Instantiable.h"
#include "Material.h"
#include "../SDK.h"

using namespace Engine::Components;


Engine::Components::Material::Material(unsigned int shaderId)
{
	this->shaderId = shaderId;
	MaterialProperties = gcnew System::Collections::Generic::Dictionary<System::String^, Locs::Generic::MaterialLoc^>();

	MaterialProperties->Add("Base Color", gcnew Locs::ColorLoc(gcnew Engine::Components::Color(0xFFFFFFFF)));
	MaterialProperties->Add("albedoMap", gcnew Locs::TextureLoc(0));
	MaterialProperties->Add("normalMap", gcnew Locs::TextureLoc(0));
}

Engine::Components::Material::Material()
{
	MaterialProperties = gcnew System::Collections::Generic::Dictionary<System::String^, Locs::Generic::MaterialLoc^>();

	MaterialProperties->Add("Base Color", gcnew Locs::ColorLoc(gcnew Engine::Components::Color(0xFFFFFFFF)));
	MaterialProperties->Add("albedoMap", gcnew Locs::TextureLoc(0));
	MaterialProperties->Add("normalMap", gcnew Locs::TextureLoc(0));
}

bool Engine::Components::Material::AddProperty(System::String^ propName, Locs::Generic::MaterialLoc^ arg)
{
	if (MaterialProperties->ContainsKey(propName))
	{
		MaterialProperties[propName] = arg;
		return true;
	}

	return false;
}

bool Engine::Components::Material::UpdateProperty(System::String^ propName, Locs::Generic::MaterialLoc^ arg)
{
	if (MaterialProperties->ContainsKey(propName))
	{
		MaterialProperties[propName] = arg;
		return true;
	}

	return false;
}

bool Engine::Components::Material::RemoveProperty(System::String^ propName)
{
	if (MaterialProperties->ContainsKey(propName))
	{
		MaterialProperties->Remove(propName);
		return true;
	}

	return false;
}

Locs::Generic::MaterialLoc^ Engine::Components::Material::GetMaterialProperty(System::String^ propName)
{
	if (MaterialProperties->ContainsKey(propName))
	{
		return MaterialProperties[propName];
	}

	return nullptr;
}

Locs::Generic::MaterialLoc^ Engine::Components::Material::GetBaseColor()
{
	if (MaterialProperties->ContainsKey("Base Color"))
	{
		return MaterialProperties["Base Color"];
	}

	return nullptr;
}

unsigned int Engine::Components::Material::GetMainTexture()
{
	if (MaterialProperties->ContainsKey("albedoMap"))
	{
		return ((Locs::TextureLoc^)MaterialProperties["albedoMap"])->textureId->Instance;
	}

	return 0;
}

unsigned int Engine::Components::Material::GetNormalMap()
{
	if (MaterialProperties->ContainsKey("normalMap"))
	{
		return ((Locs::TextureLoc^)MaterialProperties["normalMap"])->textureId->Instance;
	}

	return 0;
}

generic <class T>
T Engine::Components::Material::GetMaterialProperty(System::String^ propName)
{
	if (MaterialProperties->ContainsKey(propName))
	{
		return (T)MaterialProperties[propName];
	}

	return T();
}

void Engine::Components::Material::SetMainTexture(int textureId)
{
	if (MaterialProperties->ContainsKey("albedoMap"))
	{
		((Locs::TextureLoc^)MaterialProperties["albedoMap"])[textureId];
	}
}

void Engine::Components::Material::SetNormalMap(int textureId)
{
	if (MaterialProperties->ContainsKey("normalMap"))
	{
		((Locs::TextureLoc^)MaterialProperties["normalMap"])[textureId];
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