#pragma once
#include "Includes.h"

#include "imgui/imgui.h"
#include "CastToNative.h"

#include <vector>
#include "Object/Transform.h"
#include "DataPack.h"
#include "DataPacks.h"

namespace Engine::Internal
{
	/// <summary>
	/// Binding to the raylib abstraction layer
	/// </summary>
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class DataManager abstract
	{
	public:
		inline static GLWrapper::Shader^% HL_GetShader(unsigned int shaderId)
		{
			GLWrapper::Shader^ shader = gcnew GLWrapper::Shader();
			Shader& shaderRef = DataPacks::singleton().GetShader(shaderId);

			shader->Id = shaderRef.id;
			shader->Locs = shaderRef.locs;

			return shader;
		}

		// LOADING \\
		
		// Load a shader into the datapacks
		inline static void HL_LoadShader(unsigned int shaderId, System::String^ vertexShaderFile, System::String^ fragmentShaderFile)
		{
			Engine::Assets::Management::DataPack::singleton()->AddShader(shaderId, vertexShaderFile, fragmentShaderFile);
		}

		// Load a model into the datapacks
		inline static void HL_LoadModel(unsigned int modelId, System::String^ fileName)
		{
			Engine::Assets::Management::DataPack::singleton()->AddModel(modelId, fileName);
		}

		// Load a texture into the datapacks
		inline static void HL_LoadTexture2D(unsigned int textureId, System::String^ fileName)
		{
			Engine::Assets::Management::DataPack::singleton()->AddTextures2D(textureId, fileName);
		}

		// LOADING THROUGH POINTERS \\

		inline static void LoadShader(unsigned int shaderId, RAYLIB::Shader* shader)
		{
			Engine::Assets::Storage::DataPacks::singleton().AddShader(shaderId, *shader);
		}

		inline static void LoadModel(unsigned int modelId, RAYLIB::Model* model)
		{
			Engine::Assets::Storage::DataPacks::singleton().AddModel(modelId, *model);
		}

		inline static void LoadTexture2D(unsigned int textureId, RAYLIB::Texture2D* texture)
		{
			Engine::Assets::Storage::DataPacks::singleton().AddTexture2D(textureId, *texture);
		}

		inline static void LoadMusic(unsigned int musicId, RAYLIB::Music* music)
		{
			Engine::Assets::Storage::DataPacks::singleton().AddMusic(musicId, *music);
		}

		inline static void LoadSound(unsigned int soundId, RAYLIB::Sound* sound)
		{
			Engine::Assets::Storage::DataPacks::singleton().AddSound(soundId, *sound);
		}

		// FREEING \\

		inline static void HL_FreeModels()
		{
			Engine::Assets::Storage::DataPacks::singleton().FreeModels();
		}

		inline static void HL_FreeShaders()
		{
			Engine::Assets::Storage::DataPacks::singleton().FreeShaders();
		}

		inline static void HL_FreeAll()
		{
			Engine::Assets::Storage::DataPacks::singleton().FreeAll();
			Engine::Assets::Storage::DataPacks::singleton().LoadDefaultAssets();
		}

		inline static void HL_FreeShader(unsigned int shaderId)
		{
			Engine::Assets::Storage::DataPacks::singleton().FreeShader(shaderId);
		}

		inline static RAYLIB::Model* GetModel(unsigned int modelId)
		{
			return &Engine::Assets::Storage::DataPacks::singleton().GetModel(modelId);
		}

		inline static RAYLIB::Shader* GetShader(unsigned int shaderId)
		{
			return &Engine::Assets::Storage::DataPacks::singleton().GetShader(shaderId);
		}

		inline static RAYLIB::Texture2D* GetTexture2D(unsigned int textureId)
		{
			return &Engine::Assets::Storage::DataPacks::singleton().GetTexture2D(textureId);
		}

		inline static RAYLIB::Mesh* GetMesh(unsigned int meshId)
		{
			return Engine::Assets::Storage::DataPacks::singleton().GetMeshPtr(meshId);
		}

		inline static RAYLIB::Sound* GetSound(unsigned int soundId)
		{
			return Engine::Assets::Storage::DataPacks::singleton().GetSoundPtr(soundId);
		}

		inline static RAYLIB::Font* GetFont(unsigned int fontId)
		{
			return &Engine::Assets::Storage::DataPacks::singleton().GetFont(fontId);
		}

		inline static RAYLIB::ModelAnimation* GetAnimations(unsigned int animationId)
		{
			return Engine::Assets::Storage::DataPacks::singleton().GetAnimations(animationId);
		}

		inline static RAYLIB::ModelAnimation* GetAnimation(unsigned int animationId, unsigned int animationIndex)
		{
			return &Engine::Assets::Storage::DataPacks::singleton().GetAnimation(animationId, animationIndex);
		}

		inline static RAYLIB::Music* GetMusic(unsigned int musicId)
		{
			return Engine::Assets::Storage::DataPacks::singleton().GetMusicPtr(musicId);
		}

		inline static Engine::Components::Material^ GetMaterial(unsigned int materialId)
		{
			return Engine::Assets::Storage::DataPacks::singleton().GetMaterial(materialId);
		}
	};
}