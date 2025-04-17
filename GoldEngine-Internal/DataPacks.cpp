#include <utility>
#include "DataPacks.h"

using namespace Engine::Assets::Storage;

void onUnloadSound(RAYLIB::Sound s)
{
	UnloadSound(s);
}

void onUnloadMusicStream(RAYLIB::Music m)
{
	UnloadMusicStream(m);
}

void onUnloadModel(RAYLIB::Model m)
{
	UnloadModel(m);
}

void onUnloadShader(RAYLIB::Shader m)
{
	UnloadShader(m);
}

void onUnloadTexture(RAYLIB::Texture2D tex)
{
	UnloadTexture(tex);
}

NativeDataPack::NativeDataPack()
{
	shaders = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Shader>*>();
	models = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Model>*>();
	textures2d = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Texture2D>*>();
	sounds = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Sound>*>();
	musics = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Music>*>();
	meshes = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Mesh>*>();
	//animations = std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::ModelAnimation>*>();
	materials = std::map<unsigned int, void*>();

	fallbackModel = nullptr;
	fallbackTexture = nullptr;
	fallbackShader = nullptr;
	fallbackMaterial = nullptr;
	fallbackMesh = nullptr;
}

DataPacks* DataPacks::instance = nullptr;

DataPacks::DataPacks()
{
	nativePacks = new NativeDataPack();
	instance = this;

	if(EngineState::glInitialized)
		LoadDefaultAssets();
}

DataPacks DataPacks::singleton()
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
		nativePacks->fallbackModel = new Engine::Native::EnginePtr(RAYLIB::LoadModel("./Data/Engine/Models/Error.obj"));

	printConsole("Loaded Default Model");

	if(nativePacks->fallbackTexture == nullptr)
		nativePacks->fallbackTexture = new Engine::Native::EnginePtr(RAYLIB::LoadTexture("./Data/Engine/Textures/NotFound.png"));

	printConsole(String::Format("Loaded Default Texture (ID:{0})", nativePacks->fallbackTexture->getInstance().id));

	if(nativePacks->fallbackShader == nullptr)
		nativePacks->fallbackShader = new Engine::Native::EnginePtr(RAYLIB::LoadShader("./Data/Engine/Shaders/base.vs", "./Data/Engine/Shaders/base.fs"));

	printConsole(String::Format("Loaded Default Shader (ID:{0})", nativePacks->fallbackShader->getInstance().id));

	if (nativePacks->fallbackMaterial == nullptr)
		nativePacks->fallbackMaterial = new msclr::gcroot(gcnew Engine::Components::Material(0));

	if (nativePacks->fallbackMesh == nullptr)
		nativePacks->fallbackMesh = new Engine::Native::EnginePtr<RAYLIB::Mesh>(RAYLIB::GenMeshCube(1,1,1));
}

Texture2D& DataPacks::GetTexture2D(unsigned int textureId)
{
	try
	{
		return nativePacks->textures2d.at(textureId)->getInstance();
	}
	catch (std::exception ex)
	{
		if (nativePacks->fallbackTexture == nullptr)
			nativePacks->fallbackTexture = new Engine::Native::EnginePtr(RAYLIB::LoadTexture("./Data/Engine/Textures/NotFound.png"));

		return nativePacks->fallbackTexture->getInstance();
	}
}


Model& DataPacks::GetModel(unsigned int modelId)
{
	try
	{
		return nativePacks->models.at(modelId)->getInstance();
	}
	catch (std::exception ex)
	{
		if (nativePacks->fallbackModel == nullptr)
			nativePacks->fallbackModel = new Engine::Native::EnginePtr(RAYLIB::LoadModel("./Data/Engine/Models/Error.obj"));

		return nativePacks->fallbackModel->getInstance();
	}
}

RAYLIB::Shader& DataPacks::GetShader(unsigned int shaderId)
{
	try
	{
		return nativePacks->shaders.at(shaderId)->getInstance();
	}
	catch (std::exception e)
	{
		if (nativePacks->fallbackShader == nullptr)
			nativePacks->fallbackShader = new Engine::Native::EnginePtr(RAYLIB::LoadShader("./Data/Engine/Shaders/base.vs", "./Data/Engine/Shaders/base.fs"));

		return nativePacks->fallbackShader->getInstance();
	}
}

Engine::Components::Material^ DataPacks::GetMaterial(unsigned int materialId)
{
	try
	{
		auto sP = nativePacks->materials.at(materialId);
		return *((msclr::gcroot<Engine::Components::Material^>*)sP);
	}
	catch (std::exception ex)
	{
		return nullptr;
	}
}

Sound& DataPacks::GetSound(unsigned int soundId)
{
	try
	{
		auto sP = nativePacks->sounds.at(soundId);
		return sP->getInstance();
	}
	catch (std::exception ex)
	{
		RAYLIB::Sound emptyMusic = { 0 };
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
	catch (std::exception ex)
	{
		RAYLIB::Music emptyMusic = { 0 };
		return emptyMusic;
	}
}

Music* DataPacks::GetMusicPtr(unsigned int musicId)
{
	return &(GetMusic(musicId));
}

void DataPacks::AddMaterial(unsigned int materialId, Engine::Components::Material^ material)
{
	try
	{
		nativePacks->materials.at(materialId) = new msclr::gcroot(material);
	}
	catch (std::exception ex)
	{
		nativePacks->materials.emplace(materialId, new msclr::gcroot<Engine::Components::Material^>(msclr::gcroot(material)));
	}
}

void DataPacks::AddMusic(unsigned int soundId, RAYLIB::Music& sound)
{
	try 
	{
		nativePacks->musics.at(soundId)->setInstance(sound);
	}
	catch (std::exception ex) 
	{
		nativePacks->musics.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Music>(sound, &onUnloadMusicStream, &onUnloadMusicStream));
	}
}


void DataPacks::AddModel(unsigned int modelId, RAYLIB::Model& modelRef)
{
	try 
	{
		nativePacks->models.at(modelId)->setInstance(modelRef);
	}
	catch (std::exception ex) 
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
	catch (std::exception ex) 
	{
		nativePacks->shaders.emplace(shaderId, new Engine::Native::EnginePtr<RAYLIB::Shader>(shader, &onUnloadShader, &onUnloadShader));
	}
}

void DataPacks::AddTexture2D(unsigned int textureId, Texture2D& texture)
{
	try 
	{
		nativePacks->textures2d.at(textureId)->setInstance(texture);
	}
	catch (std::exception ex) 
	{
		nativePacks->textures2d.emplace(textureId, new Engine::Native::EnginePtr<RAYLIB::Texture2D>(texture, &onUnloadTexture, &onUnloadTexture));
	}
}


void DataPacks::AddSound(unsigned int soundId, RAYLIB::Sound& sound)
{
	try
	{
		nativePacks->sounds.at(soundId)->setInstance(sound);
	}
	catch (std::exception ex)
	{
		nativePacks->sounds.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Sound>(sound, &onUnloadSound, &onUnloadSound));
	}
}

RAYLIB::Mesh& DataPacks::GetMesh(unsigned int musicId)
{
	try
	{
		auto sP = nativePacks->meshes.at(musicId);
		return sP->getInstance();
	}
	catch (std::exception ex)
	{
		if (nativePacks->fallbackMesh == nullptr)
			nativePacks->fallbackMesh = new Engine::Native::EnginePtr<RAYLIB::Mesh>(nativePacks->fallbackModel->getInstance().meshes[0]);

		return nativePacks->fallbackMesh->getInstance();
	}
}

void DataPacks::AddMesh(unsigned int soundId, RAYLIB::Mesh& sound)
{
	try
	{
		nativePacks->meshes.at(soundId)->setInstance(sound);
	}
	catch (std::exception ex)
	{
		nativePacks->meshes.emplace(soundId, new Engine::Native::EnginePtr<RAYLIB::Mesh>(sound, &RAYLIB::UnloadMesh, &RAYLIB::UnloadMesh));
	}
}

RAYLIB::Mesh* DataPacks::GetMeshPtr(unsigned int musicId)
{
	return &(GetMesh(musicId));
}