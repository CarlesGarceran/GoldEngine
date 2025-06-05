#include "../SDK.h"
#include "Skybox.h"

typedef enum MaterialLocations
{
	ColorLoc = 0,
	TextureLoc = 1,
	FloatLoc = 2
};

void Engine::EngineObjects::Skybox::Init(unsigned int modelId, unsigned int matId, unsigned int texturePath)
{
	material = matId;
	texture = texturePath;
	model = modelId;
}

void Engine::EngineObjects::Skybox::Draw()
{
	auto t = getTransform();

	Engine::Components::Material^ materialPtr = DataPacks::singleton().GetMaterial(material);
	RAYLIB::Model modelCopy = DataPacks::singleton().GetModel(model);
	RAYLIB::Shader shader = DataPacks::singleton().GetShader(materialPtr->shaderId->getInstance());

	modelCopy.transform = RAYMATH::MatrixRotateXYZ({
		DEG2RAD * this->transform->scale->x,
		DEG2RAD * this->transform->scale->y,
		DEG2RAD * this->transform->scale->z
		});

	modelCopy.materials[0].shader = shader;

	for each (auto key in materialPtr->MaterialProperties)
	{
		Engine::Components::Locs::Generic::MaterialLoc^ genericLoc = key->GetMaterialLocation();

		int locType = genericLoc->GetLocType();

		switch (locType)
		{
		case ColorLoc:
			auto array = ((Engine::Components::Locs::ColorLoc^)genericLoc)->color->toFloat();
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), array.data(), RAYLIB::SHADER_ATTRIB_VEC4);
			break;
		case TextureLoc:
			RAYLIB::Texture2D texture2D = DataPacks::singleton().GetTexture2D(((Engine::Components::Locs::TextureLoc^)genericLoc)->textureId);
			SetShaderValueTexture(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), texture2D);
			break;
		case FloatLoc:
			float value = ((Engine::Components::Locs::FloatLoc^)genericLoc)->value;
			SetShaderValue(shader, GetShaderLocation(shader, CastStringToNative(key->locName).c_str()), &value, RAYLIB::SHADER_ATTRIB_FLOAT);
			break;
		}
	}

	DrawModelEx(modelCopy, t->position->toNative(), { 0,0,0 }, 0.0f, t->scale->toNative(), GetColor(0xFFFFFFFF));
}