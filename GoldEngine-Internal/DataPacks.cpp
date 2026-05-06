#include <utility>
#include "../SDK.h"
#include "DataPacks.h"
#include "../LoggingAPI.h"

using namespace Engine::Assets::Storage;

void UnloadAnimationStruct(AnimationStruct animStruct)
{
	RAYLIB::UnloadModelAnimations(animStruct.animations, animStruct.animationCount);
}

void onUnloadTexture(RAYLIB::Texture2D tex)
{
	if (RAYLIB::IsTextureValid(tex))
	{
		RAYLIB::UnloadTexture(tex);
	}
}

void onUnloadShader(RAYLIB::Shader shader)
{
	if (RAYLIB::IsShaderValid(shader))
	{
		RAYLIB::UnloadShader(shader);
	}
}

void onUnloadModel(RAYLIB::Model model)
{
	if(RAYLIB::IsModelValid(model))
	{
		RAYLIB::UnloadModel(model);
	}
}

NativeDataPack::NativeDataPack()
{
	shaders = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Shader>*>();
	models = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Model>*>();
	textures2d = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Texture2D>*>();
	sounds = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Sound>*>();
	musics = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Music>*>();
	meshes = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Mesh>*>();
	animations = std::map<unsigned int, Engine::Native::EnginePtr<AnimationStruct>*>();
	fonts = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Font>*>();
	materials = std::map<unsigned int, msclr::gcroot<Engine::Components::Material^>*>();

	fallbackModel = nullptr;
	fallbackTexture = nullptr;
	fallbackShader = nullptr;
	fallbackMaterial = nullptr;
	fallbackMesh = nullptr;
}

Engine::Assets::Storage::NativeDataPack::~NativeDataPack()
{
	if (fallbackModel != nullptr)
	{
		delete fallbackModel;
		fallbackModel = nullptr;
	}

	if (fallbackTexture != nullptr)
	{
		delete fallbackTexture;
		fallbackTexture = nullptr;
	}

	if (fallbackShader != nullptr)
	{
		delete fallbackShader;
		fallbackShader = nullptr;
	}

	if (fallbackMaterial != nullptr)
	{
		delete fallbackMaterial;
		fallbackMaterial = nullptr;
	}

	if (fallbackMesh != nullptr)
	{
		delete fallbackMesh;
		fallbackMesh = nullptr;
	}

	if (fallbackFont != nullptr)
	{
		delete fallbackFont;
		fallbackFont = nullptr;
	}

	for (auto& shader : shaders)
	{
		print("[Native DataPacks]:", String::Format("Unloading ShaderID: {0}", shader.first));
		delete shader.second;
	}
	shaders.clear();

	for (auto& model : models)
	{
		print("[Native DataPacks]:", String::Format("Unloading ModelID: {0}", model.first));
		delete model.second;
	}
	models.clear();

	for (auto& texture : textures2d)
	{
		print("[Native DataPacks]:", String::Format("Unloading TextureID: {0}", texture.first));
		delete texture.second;
	}
	textures2d.clear();

	for (auto& sound : sounds)
	{
		print("[Native DataPacks]:", String::Format("Unloading SoundID: {0}", sound.first));
		delete sound.second;
	}
	sounds.clear();

	for (auto& music : musics)
	{
		print("[Native DataPacks]:", String::Format("Unloading MusicID: {0}", music.first));
		delete music.second;
	}
	musics.clear();

	for (auto& mesh : meshes)
	{
		print("[Native DataPacks]:", String::Format("Unloading MeshID: {0}", mesh.first));
		delete mesh.second;
	}
	meshes.clear();

	for (auto& animation : animations)
	{
		print("[Native DataPacks]:", String::Format("Unloading AnimationID: {0}", animation.first));
		delete animation.second;
	}
	animations.clear();

	for (auto& font : fonts)
	{
		print("[Native DataPacks]:", String::Format("Unloading FontID: {0}", font.first));
		delete font.second;
	}
	fonts.clear();

	for (auto& material : materials)
	{
		print("[Native DataPacks]:", String::Format("Unloading MaterialID: {0}", material.first));
		delete material.second;
	}
	materials.clear();
}

DataPacks* DataPacks::instance = nullptr;

DataPacks::DataPacks()
{
	nativePacks = new NativeDataPack();
	instance = this;

	if(EngineState::glInitialized)
		LoadDefaultAssets();
}

void DataPacks::FreeAll()
{
	if (nativePacks != nullptr) 
	{
		delete nativePacks;
		nativePacks = nullptr;
	}

	nativePacks = new NativeDataPack();
	instance = this;

	LoadDefaultAssets();
}

DataPacks& DataPacks::singleton()
{
	return *instance;
}

NativeDataPack* DataPacks::GetNativeDataPack()
{
	return nativePacks;
}

void DataPacks::LoadDefaultAssets()
{
	printConsole("Loading Default Assets");

	if(nativePacks->fallbackModel == nullptr)
		nativePacks->fallbackModel = new Engine::Native::EnginePtr<RAYLIB::Model>(RAYLIB::LoadModel("./Data/Engine/Models/Error.obj"), &onUnloadModel);

	if(nativePacks->fallbackTexture == nullptr)
		nativePacks->fallbackTexture = new Engine::Native::EnginePtr<RAYLIB::Texture2D>(RAYLIB::LoadTexture("./Data/Engine/Textures/NotFound.png"), &onUnloadTexture);

	if(nativePacks->fallbackShader == nullptr)
		nativePacks->fallbackShader = new Engine::Native::EnginePtr<RAYLIB::Shader>(RAYLIB::LoadShader("./Data/Engine/Shaders/base.vs", "./Data/Engine/Shaders/base.fs"), &onUnloadShader);

	if (nativePacks->fallbackMaterial == nullptr)
		nativePacks->fallbackMaterial = new msclr::gcroot(gcnew Engine::Components::Material(0));

	if (nativePacks->fallbackFont == nullptr)
		nativePacks->fallbackFont = new Engine::Native::EnginePtr<RAYLIB::Font>(RAYLIB::GetFontDefault());

	if (nativePacks->fallbackMesh == nullptr)
		nativePacks->fallbackMesh = new Engine::Native::EnginePtr<RAYLIB::Mesh>(RAYLIB::GenMeshCube(1,1,1), &UnloadMesh);
}

Texture2D& DataPacks::GetTexture2D(unsigned int textureId)
{
	try
	{
		return nativePacks->textures2d.at(textureId)->getInstance();
	}
	catch (const std::exception& ex)
	{
		return nativePacks->fallbackTexture->getInstance();
	}
}


Model& DataPacks::GetModel(unsigned int modelId)
{
	try
	{
		return nativePacks->models.at(modelId)->getInstance();
	}
	catch (const std::exception& ex)
	{
		return nativePacks->fallbackModel->getInstance();
	}
}

RAYLIB::Shader& DataPacks::GetShader(unsigned int shaderId)
{
	try
	{
		return nativePacks->shaders.at(shaderId)->getInstance();
	}
	catch (std::exception ex)
	{
		return nativePacks->fallbackShader->getInstance();
	}
}

Engine::Components::Material^ DataPacks::GetMaterial(unsigned int materialId)
{
	try
	{
		return *nativePacks->materials.at(materialId);
	}
	catch (const std::exception& ex)
	{
		return *nativePacks->fallbackMaterial;
	}
}

Sound& DataPacks::GetSound(unsigned int soundId)
{
	try
	{
		auto sP = nativePacks->sounds.at(soundId);
		return sP->getInstance();
	}
	catch (const std::exception& ex)
	{
		static RAYLIB::Sound emptyMusic{};
		return emptyMusic;
	}
}

Sound* DataPacks::GetSoundPtr(unsigned int soundId)
{
	return &(GetSound(soundId));
}


Music& DataPacks::GetMusic(unsigned int musicId)
{
	try
	{
		auto sP = nativePacks->musics.at(musicId);
		return sP->getInstance();
	}
	catch (const std::exception& ex)
	{
		static RAYLIB::Music emptyMusic{};
		return emptyMusic;
	}
}

Music* DataPacks::GetMusicPtr(unsigned int musicId)
{
	return &(GetMusic(musicId));
}

void DataPacks::AddMaterial(unsigned int materialId, Engine::Components::Material^ material)
{
	auto it = nativePacks->materials.find(materialId);
	if (it != nativePacks->materials.end())
	{
		delete it->second;
		it->second = new msclr::gcroot(material);
	}
	else
	{
		nativePacks->materials.emplace(
			materialId,
			new msclr::gcroot<Engine::Components::Material^>(material)
		);
	}
}
void DataPacks::AddMusic(unsigned int soundId, RAYLIB::Music& sound)
{
	try 
	{
		nativePacks->musics.at(soundId)->setInstanceRef(sound);
	}
	catch (const std::exception& ex) 
	{
		nativePacks->musics.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Music>(sound, &UnloadMusicStream, &UnloadMusicStream));
	}
}

RAYLIB::Font& Engine::Assets::Storage::DataPacks::GetFont(unsigned int fontId)
{
	try
	{
		return nativePacks->fonts.at(fontId)->getInstance();
	}
	catch (const std::exception& ex)
	{
		if (nativePacks->fallbackFont == nullptr)
			nativePacks->fallbackFont = new Engine::Native::EnginePtr(RAYLIB::GetFontDefault());

		return nativePacks->fallbackFont->getInstance();
	}
}

void DataPacks::AddFont(unsigned int fontId, RAYLIB::Font& font)
{
	try
	{
		nativePacks->fonts.at(fontId)->setInstanceRef(font);
	}
	catch (const std::exception& ex)
	{
		nativePacks->fonts.emplace(fontId, new Engine::Native::EnginePtr<RAYLIB::Font>(font, &UnloadFont, &UnloadFont));
	}
}

AnimationStruct Engine::Assets::Storage::DataPacks::GetAnimationStruct(unsigned int animationId)
{
	try
	{
		auto sP = nativePacks->animations.at(animationId);
		return sP->getInstance();
	}
	catch (const std::exception& ex)
	{
		return {};
	}
}

RAYLIB::ModelAnimation* Engine::Assets::Storage::DataPacks::GetAnimations(unsigned int animationId)
{
	try
	{
		auto sP = nativePacks->animations.at(animationId);
		return sP->getInstance().animations;
	}
	catch (const std::exception& ex)
	{
		return {};
	}
}

RAYLIB::ModelAnimation Engine::Assets::Storage::DataPacks::GetAnimation(unsigned int animationId, unsigned int animationIndex)
{
	try
	{
		auto sP = nativePacks->animations.at(animationId);
		return sP->getInstance().animations[animationIndex];
	}
	catch (const std::exception& ex)
	{
		return {};
	}
}

void Engine::Assets::Storage::DataPacks::AddAnimations(unsigned int animationId, AnimationStruct& animations)
{
	try
	{
		nativePacks->animations.at(animationId)->setInstanceRef(animations);
	}
	catch (const std::exception& ex)
	{
		nativePacks->animations.emplace(animationId, new Engine::Native::EnginePtr<AnimationStruct>(animations, &UnloadAnimationStruct, &UnloadAnimationStruct));
	}
}

void DataPacks::AddModel(unsigned int modelId, RAYLIB::Model& modelRef)
{
	try 
	{
		nativePacks->models.at(modelId)->setInstanceRef(modelRef);
	}
	catch (const std::exception& ex) 
	{
		nativePacks->models.emplace(modelId, new Engine::Native::EnginePtr<RAYLIB::Model>(modelRef, &onUnloadModel, &onUnloadModel));
	}
}

void DataPacks::AddShader(unsigned int shaderId, Shader& shader)
{
	try 
	{
		nativePacks->shaders.at(shaderId)->setInstance(shader);
	}
	catch (const std::exception& ex) 
	{
		nativePacks->shaders.emplace(shaderId, new Engine::Native::EnginePtr<RAYLIB::Shader>(shader, &onUnloadShader, &onUnloadShader));
	}
}

void DataPacks::AddTexture2D(unsigned int textureId, Texture2D& texture)
{
	try 
	{
		nativePacks->textures2d.at(textureId)->setInstanceRef(texture);
	}
	catch (const std::exception& ex) 
	{
		nativePacks->textures2d.emplace(textureId, new Engine::Native::EnginePtr<RAYLIB::Texture2D>(texture, &onUnloadTexture, &onUnloadTexture));
	}
}


void DataPacks::AddSound(unsigned int soundId, RAYLIB::Sound& sound)
{
	try
	{
		nativePacks->sounds.at(soundId)->setInstanceRef(sound);
	}
	catch (const std::exception& ex)
	{
		nativePacks->sounds.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Sound>(sound, &UnloadSound, &UnloadSound));
	}
}

RAYLIB::Shader& Engine::Assets::Storage::DataPacks::GetFallbackShader()
{
	return nativePacks->fallbackShader->getInstance();
}

RAYLIB::Model& Engine::Assets::Storage::DataPacks::GetFallbackModel()
{
	return nativePacks->fallbackModel->getInstance();
}

RAYLIB::Texture2D& Engine::Assets::Storage::DataPacks::GetFallbackTexture()
{
	return nativePacks->fallbackTexture->getInstance();
}

RAYLIB::Mesh& DataPacks::GetMesh(unsigned int musicId)
{
	try
	{
		auto sP = nativePacks->meshes.at(musicId);
		return sP->getInstance();
	}
	catch (const std::exception& ex)
	{
		return nativePacks->fallbackMesh->getInstance();
	}
}

void DataPacks::AddMesh(unsigned int soundId, RAYLIB::Mesh& sound)
{
	try
	{
		nativePacks->meshes.at(soundId)->setInstanceRef(sound);
	}
	catch (const std::exception& ex)
	{
		nativePacks->meshes.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Mesh>(sound, &RAYLIB::UnloadMesh, &RAYLIB::UnloadMesh));
	}
}

RAYLIB::Mesh* DataPacks::GetMeshPtr(unsigned int musicId)
{
	return &(GetMesh(musicId));
}
