#pragma once
#include <vector>
#include <algorithm>
#include <utility>
#include <memory>
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
	class NativeDataPack
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
		
		std::map<unsigned int, msclr::gcroot<Engine::Components::Material^>*> materials;

		Engine::Native::EnginePtr<RAYLIB::Shader>* fallbackShader = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Model>* fallbackModel = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Texture2D>* fallbackTexture = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Font>* fallbackFont = nullptr;
		msclr::gcroot<Engine::Components::Material^>* fallbackMaterial = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Mesh>* fallbackMesh = nullptr;

		NativeDataPack();
		~NativeDataPack();
	};

	public class DataPacks
	{
	private:
		NativeDataPack* nativePacks;

	public:
		DataPacks();
		
		void FreeAll();

		void LoadDefaultAssets();

		bool dataPackHasAssets()
		{
			int assetCount = 0;
			
			assetCount += nativePacks->shaders.size();
			assetCount += nativePacks->models.size();
			assetCount += nativePacks->textures2d.size();
			assetCount += nativePacks->materials.size();
			assetCount += nativePacks->meshes.size();
			assetCount += nativePacks->sounds.size();
			assetCount += nativePacks->musics.size();
			assetCount += nativePacks->fonts.size();
			assetCount += nativePacks->animations.size();

			return (assetCount > 0);
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

		AnimationStruct GetAnimationStruct(unsigned int animationId);
		RAYLIB::ModelAnimation* GetAnimations(unsigned int animationId);
		RAYLIB::ModelAnimation GetAnimation(unsigned int animationId, unsigned int animationIndex);
		void AddAnimations(unsigned int animationId, AnimationStruct& animations);

		bool HasTexture2D(unsigned int textureId)
		{
			return nativePacks->textures2d.find(textureId) != nativePacks->textures2d.end();
		}

		bool HasShader(unsigned int shaderId)
		{
			return nativePacks->shaders.find(shaderId) != nativePacks->shaders.end();
		}

		bool HasModel(unsigned int materialID)
		{
			return nativePacks->models.find(materialID) != nativePacks->models.end();
		}

		static DataPacks* instance;

		static DataPacks& singleton();

		NativeDataPack* GetNativeDataPack();
	};

}