#pragma once
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include "Includes.h"
#include "GlIncludes.h"
#include "Object/Material.h"
#include "Object/Transform.h"
#include "LoggingAPI.h"
#include "AnimationStruct.h"

/*
	DATAPACKS
	Used for storing game contents in memory.
*/

namespace Engine::Assets::Storage
{
	public class NativeDataPack
	{
	public:
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Shader>*> shaders;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Model>*> models;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Texture2D>*> textures2d;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Sound>*> sounds;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Music>*> musics;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Mesh>*> meshes;
		std::map<unsigned int, Engine::Native::EnginePtr<AnimationStruct>*> animations;
		std::map<unsigned int, Engine::Native::EnginePtr<RAYLIB::Font>*> fonts;
		
		std::map<unsigned int, void*> materials;

		Engine::Native::EnginePtr<RAYLIB::Shader>* fallbackShader = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Model>* fallbackModel = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Texture2D>* fallbackTexture = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Font>* fallbackFont = nullptr;
		msclr::gcroot<Engine::Components::Material^>* fallbackMaterial = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Mesh>* fallbackMesh = nullptr;

		NativeDataPack();
	};

	public class DataPacks
	{
	private:
		NativeDataPack* nativePacks;

	public:
		DataPacks();

		void LoadDefaultAssets();

		bool dataPackHasAssets()
		{
			int assetCount = 0;

			assetCount += nativePacks->shaders.size();
			assetCount += nativePacks->models.size();
			assetCount += nativePacks->textures2d.size();
			assetCount += nativePacks->materials.size();
			//assetCount += nativePacks->meshes.size();
			assetCount += nativePacks->sounds.size();
			assetCount += nativePacks->musics.size();
			assetCount += nativePacks->fonts.size();
			assetCount += nativePacks->animations.size();

			return (assetCount > 0);
		}

		void FreeShaders()
		{
			for (int x = 0; x < nativePacks->shaders.size(); x++)
			{
				auto sP = nativePacks->shaders[x];

				delete sP;
			}

			nativePacks->shaders.clear();
		}

		void FreeMusics()
		{
			for (int x = 0; x < nativePacks->musics.size(); x++)
			{
				auto sP = nativePacks->musics[x];

				delete sP;
			}

			nativePacks->musics.clear();
		}


		void FreeSounds()
		{
			for (int x = 0; x < nativePacks->sounds.size(); x++)
			{
				auto sP = nativePacks->sounds[x];

				delete sP;
			}

			nativePacks->sounds.clear();
		}


		void FreeShader(unsigned int shaderId)
		{
			try
			{
				auto sP = nativePacks->shaders.at(shaderId);
				delete sP;
			}
			catch (std::exception ex)
			{

			}

			nativePacks->shaders.erase(shaderId);
		}

		void FreeMaterials()
		{
			for (int x = 0; x < nativePacks->materials.size(); x++)
			{
				if (nativePacks->materials.at(x) != nullptr)
				{
					delete nativePacks->materials[x];
				}
			}

			nativePacks->materials.clear();
		}

		void FreeModels()
		{
			for (int x = 0; x < nativePacks->models.size(); x++)
			{
				auto sP = nativePacks->models[x];

				delete sP;
			}

			nativePacks->models.clear();
		}

		void FreeTextures2D()
		{
			for (int x = 0; x < nativePacks->textures2d.size(); x++)
			{
				auto tP = nativePacks->textures2d[x];

				delete tP;
			}

			nativePacks->textures2d.clear();
		}

		void FreeAnimations()
		{
			for (int x = 0; x < nativePacks->animations.size(); x++)
			{
				auto sP = nativePacks->animations[x];

				sP->destroy();
			}

			nativePacks->models.clear();
		}

		void FreeAll()
		{
			if (this->nativePacks == nullptr)
				return;

			printf("Freeing and unloading loaded assets\n");

			FreeModels();
			FreeTextures2D();
			FreeMaterials();
			FreeShaders();
			FreeSounds();
			FreeMusics();
			FreeAnimations();
		}

		RAYLIB::Model& GetFallbackModel();
		RAYLIB::Texture2D& GetFallbackTexture();

		RAYLIB::Mesh& GetMesh(unsigned int meshId);
		RAYLIB::Mesh* GetMeshPtr(unsigned int meshId);
		void AddMesh(unsigned int meshId, RAYLIB::Mesh& mesh);

		Texture2D& GetTexture2D(unsigned int textureId);
		void AddTexture2D(unsigned int textureId, Texture2D& texture);

		Sound& GetSound(unsigned int soundId);
		Sound* GetSoundPtr(unsigned int soundId);
		void AddSound(unsigned int soundId, RAYLIB::Sound& sound);

		Music& GetMusic(unsigned int musicId);
		Music* GetMusicPtr(unsigned int musicId);
		void AddMusic(unsigned int musicId, RAYLIB::Music& music);

		Model& GetModel(unsigned int modelId);
		void AddModel(unsigned int modelId, RAYLIB::Model& modelRef);

		RAYLIB::Shader& GetShader(unsigned int shaderId);
		void AddShader(unsigned int shaderId, RAYLIB::Shader& shader);

		Engine::Components::Material^ GetMaterial(unsigned int materialId);
		void AddMaterial(unsigned int materialId, Engine::Components::Material^ material);

		RAYLIB::Font& GetFont(unsigned int fontId);
		void AddFont(unsigned int fontId, RAYLIB::Font& font);

		RAYLIB::ModelAnimation* GetAnimations(unsigned int animationId);
		RAYLIB::ModelAnimation GetAnimation(unsigned int animationId, unsigned int animationIndex);
		void AddAnimations(unsigned int animationId, AnimationStruct& animations);

		bool HasTexture2D(unsigned int textureId)
		{
			bool hasTexture2D = false;


			auto sP = &nativePacks->textures2d[textureId];

			if (sP != nullptr)
				hasTexture2D = true;

			return hasTexture2D;
		}

		bool HasShader(unsigned int shaderId)
		{
			bool hasShader = false;

			auto sP = &nativePacks->shaders[shaderId];

			if (sP != nullptr)
			{
				hasShader = true;
			}

			return hasShader;
		}

		bool HasModel(unsigned int materialID)
		{
			bool hasMaterial = false;

			auto sP = &nativePacks->models[materialID];

			if (sP != nullptr)
				hasMaterial = true;


			return hasMaterial;
		}

		static DataPacks* instance;

		static DataPacks singleton();

		NativeDataPack* GetNativeDataPack();
	};

}