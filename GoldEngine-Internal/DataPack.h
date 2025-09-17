#pragma once
#include "Includes.h"
#include "CastToNative.h"
#include "CypherLib.h"
#include "DataPacks.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

namespace Engine::Assets::Management
{
	typedef enum assetType { _Model, _Shader, _Texture2D, _Material, _Sound, _Musics, _Fonts, _Animations };

	public ref class DataPack
	{
	private:
		String^ fileTarget;
		static DataPack^ singletonRef = nullptr;

	private:
		void ParseContentData()
		{
			print("[DataPack]:", "Adding stored assets to the datapacks");

			for each(unsigned int shader_id in shaders->Keys)
			{
				auto filePath = shaders[shader_id];

				AddShader(shader_id, filePath[0], filePath[1]);
			}

			for each(unsigned int modelId in models->Keys)
			{
				System::String^ filePath = models[modelId];

				AddModel(modelId, filePath);
			}

			for each(unsigned int materialId in materials->Keys)
			{
				System::String^ filePath = materials[materialId];

				AddMaterial(materialId, filePath);
			}

			for each(unsigned int textureId in textures2d->Keys)
			{
				System::String^ filePath = textures2d[textureId];

				AddTextures2D(textureId, filePath);
			}

			for each(unsigned int soundId in sounds->Keys)
			{
				System::String^ filePath = sounds[soundId];

				AddSound(soundId, filePath);
			}

			for each(unsigned int musicId in musics->Keys)
			{
				System::String^ filePath = musics[musicId];

				AddMusic(musicId, filePath);
			}

			for each(unsigned int fontId in fonts->Keys)
			{
				System::String^ filePath = fonts[fontId];

				AddFont(fontId, filePath);
			}

			for each(unsigned int animationId in fonts->Keys)
			{
				System::String^ filePath = fonts[animationId];

				AddModelAnimation(animationId, filePath);
			}
		}

		bool AssetExists(System::String^ fN)
		{
			return File::Exists("Data/" + fN + ".asset");
		}

		void CloneDataPack(DataPack^ newPack)
		{
			shaders = gcnew Dictionary<unsigned int, array<String^>^>(newPack->shaders);
			models = gcnew Dictionary<unsigned int, String^>(newPack->models);
			textures2d = gcnew Dictionary<unsigned int, String^>(newPack->textures2d);
			materials = gcnew Dictionary<unsigned int, String^>(newPack->materials);
			sounds = gcnew Dictionary<unsigned int, String^>(newPack->sounds);
			musics = gcnew Dictionary<unsigned int, String^>(newPack->musics);
			fonts = gcnew Dictionary<unsigned int, String^>(newPack->fonts);
			modelAnimations = gcnew Dictionary<unsigned int, String^>(newPack->modelAnimations);

			ParseContentData();
		}

		void Create()
		{
			shaders = gcnew Dictionary<unsigned int, array<String^>^>();
			models = gcnew Dictionary<unsigned int, String^>();
			materials = gcnew Dictionary<unsigned int, String^>();
			textures2d = gcnew Dictionary<unsigned int, String^>();
			sounds = gcnew Dictionary<unsigned int, String^>();
			musics = gcnew Dictionary<unsigned int, String^>();
			fonts = gcnew Dictionary<unsigned int, String^>();
			modelAnimations = gcnew Dictionary<unsigned int, String^>();
		}

	public:
		System::Collections::Generic::Dictionary<unsigned int, array<String^>^>^ shaders;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ models;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ materials;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ textures2d;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ sounds;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ musics;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ fonts;
		System::Collections::Generic::Dictionary<unsigned int, String^>^ modelAnimations;

	public:
		DataPack(String^ fileName)
		{
			singletonRef = this;

			shaders = gcnew Dictionary<unsigned int, array<String^>^>();
			models = gcnew Dictionary<unsigned int, String^>();
			materials = gcnew Dictionary<unsigned int, String^>();
			textures2d = gcnew Dictionary<unsigned int, String^>();
			sounds = gcnew Dictionary<unsigned int, String^>();
			musics = gcnew Dictionary<unsigned int, String^>();
			fonts = gcnew Dictionary<unsigned int, String^>();
			modelAnimations = gcnew Dictionary<unsigned int, String^>();
			singletonRef = this;
			fileTarget = fileName;
		}

		Shader AddShader(unsigned int id, String^ vs, String^ fs)
		{
			if (!shaders->ContainsKey(id))
			{
				auto sstring = gcnew array<String^>(2);

				sstring[0] = (vs);
				sstring[1] = (fs);

				shaders->Add(id, sstring);
			}

			std::string vertexShader = "";
			std::string fragmentShader = "";

			vertexShader = CastStringToNative(vs);
			fragmentShader = CastStringToNative(fs);

			Shader s = LoadShader(vertexShader.c_str(), fragmentShader.c_str());

			print("[Resource Manager]:", "Compiling Stored Shader [" + id + "]");
			print("[Resource Manager]:", "VertexShader: " + vs);
			print("[Resource Manager]:", "FragmentShader: " + fs);

			if (s.id == NULL)
			{
				print("[Resource Manager]:", "Failed to compile shader [" + id + "]");
				print("[Resource Manager]", "------------------------------------------");
				return Engine::Assets::Storage::DataPacks::singleton().GetNativeDataPack()->fallbackShader->getInstance();
			}

			print("[Resource Manager]:", "Compiled Shader id -> " + s.id);

			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddShader(id, s);

			return s;
		}

		Engine::Components::Material^ AddMaterial(unsigned int id, String^ path)
		{
			if (!materials->ContainsKey(id))
				materials->Add(id, path);

			Engine::Components::Material^ material = Deserialize<Engine::Components::Material^>(File::ReadAllText(path));
			material->DeserializeProperties();

			print("[Resource Manager]:", "Loading Material");

			print("[Resource Manager]:", "Path -> " + path);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddMaterial(id, material);

			return material;
		}

		Model AddModel(unsigned int id, String^ path)
		{
			if (!models->ContainsKey(id))
				models->Add(id, path);

			std::string text = "";

			text = CastStringToNative(path);

			Model m = LoadModel(text.c_str());

			print("[Resource Manager]:", "Loading Model");

			print("[Resource Manager]:", "Path -> " + path);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddModel(id, m);

			return m;
		}

		Sound AddSound(unsigned int id, String^ sound)
		{
			if (!sounds->ContainsKey(id))
				sounds->Add(id, sound);

			std::string text = "";

			text = CastStringToNative(sound);

			print("[Resource Manager]:", "Loading Sound");

			print("[Resource Manager]:", "Path -> " + sound);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			RAYLIB::Wave wave = LoadWave(text.c_str());

			Sound _sound = LoadSoundFromWave(wave);

			Engine::Assets::Storage::DataPacks::singleton().AddSound(id, _sound);

			return _sound;
		}

		Music AddMusic(unsigned int id, String^ sound)
		{
			if (!musics->ContainsKey(id))
			{
				musics->Add(id, sound);
			}

			std::string text = "";

			text = CastStringToNative(sound);

			Music _sound = LoadMusicStream(text.c_str());

			print("[Resource Manager]:", "Loading Music");

			print("[Resource Manager]:", "Path -> " + sound);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddMusic(id, _sound);

			return _sound;
		}

		Font AddFont(unsigned int id, String^ font)
		{
			if (!fonts->ContainsKey(id))
			{
				fonts->Add(id, font);
			}

			std::string text = "";

			text = CastStringToNative(font);

			Font _font = LoadFont(text.c_str());

			print("[Resource Manager]:", "Loading Font");

			print("[Resource Manager]:", "Path -> " + font);
			print("[Resource Manager]:", "Id -> " + id);


			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddFont(id, _font);

			return _font;
		}

		AnimationStruct AddModelAnimation(unsigned int id, String^ path)
		{
			if (!modelAnimations->ContainsKey(id))
			{
				modelAnimations->Add(id, path);
			}

			std::string text = "";

			text = CastStringToNative(path);

			AnimationStruct* animStruct = new AnimationStruct{ 0 };

			animStruct->animations = RAYLIB::LoadModelAnimations(text.c_str(), &animStruct->animationCount);

			print("[Resource Manager]:", "Loading Animation");

			print("[Resource Manager]:", "Path -> " + path);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			Engine::Assets::Storage::DataPacks::singleton().AddAnimations(id, *animStruct);

			return *animStruct;
		}

		Texture2D AddTextures2D(unsigned int id, String^ tex)
		{
			if (!textures2d->ContainsKey(id))
			{
				textures2d->Add(id, tex);
			}

			std::string text = "";

			text = CastStringToNative(tex);

			print("[Resource Manager]:", "Loading Texture2D");

			print("[Resource Manager]:", "Path -> " + tex);
			print("[Resource Manager]:", "Id -> " + id);

			print("[Resource Manager]", "------------------------------------------");

			Texture2D texture = LoadTexture(text.c_str());

			Engine::Assets::Storage::DataPacks::singleton().AddTexture2D(id, texture);

			return texture;
		}

		bool hasAsset(assetType aTyp, unsigned int value)
		{
			switch (aTyp)
			{
			case _Shader:
				for each(auto T in shaders)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Material:
				for each(auto T in materials)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Model:
				for each(auto T in models)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Texture2D:
				for each(auto T in textures2d)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Sound:
				for each(auto T in sounds)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Musics:
				for each(auto T in musics)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Fonts:
				for each (auto T in fonts)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			case _Animations:
				for each (auto T in modelAnimations)
				{
					if (T.Key == value)
					{
						return true;
					}
				}
				return false;
				break;
			}
		}

		std::tuple<bool, int> hasAsset(assetType aTyp, String^ value)
		{
			switch (aTyp)
			{
			case _Shader:
				for each(auto T in shaders)
				{
					if (T.Value[0] == value || T.Value[1] == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Material:
				for each(auto T in materials)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Model:
				for each(auto T in models)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Texture2D:
				for each(auto T in textures2d)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Sound:
				for each(auto T in sounds)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Musics:
				for each(auto T in musics)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Fonts:
				for each (auto T in fonts)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			case _Animations:
				for each (auto T in modelAnimations)
				{
					if (T.Value == value)
					{
						return std::tuple<bool, int>(true, T.Key);
					}
				}
				return std::tuple<bool, int>(false, 0);
				break;
			}
		}

		unsigned int GetAssetID(assetType type)
		{
			unsigned int assetId = 0;

			switch (type)
			{
			case _Material:
				for each(auto T in materials)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Model:
				for each(auto T in models)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Shader:
				for each(auto T in shaders)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Texture2D:
				for each(auto T in textures2d)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Sound:
				for each(auto T in sounds)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Musics:
				for each(auto T in musics)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Fonts:
				for each (auto T in fonts)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			case _Animations:
				for each (auto T in modelAnimations)
				{
					assetId = Math::Max(assetId, T.Key + 1);
				}
				break;
			}

			return assetId;
		}

		void setFile(String^ fileName)
		{
			fileTarget = fileName;
		}

		String^ getFile()
		{
			return fileTarget;
		}

		String^ getFileContents()
		{
			return File::ReadAllText(getFile());
		}

		void WriteToFile(System::String^ fileName, unsigned int password)
		{
			if (AssetExists(fileName))
			{
				String^ serializedData = Newtonsoft::Json::JsonConvert::SerializeObject(this, Newtonsoft::Json::Formatting::Indented);
				String^ cipheredContents = Engine::Encryption::CypherLib::EncryptString(serializedData, password);

				//System::IO::File::WriteAllText("Data/" + scene->sceneName + ".scn", System::Convert::ToBase64String(Encoding::UTF8->GetBytes(cipheredContents)));
				System::IO::File::WriteAllText("Data/" + fileName + ".asset", serializedData);
			}
			else
			{
				// create file and rerun save
				System::IO::File::Create("Data/" + fileName + ".asset")->Close();
				WriteToFile(fileName, password);
			}
		}

		void ReadFromFile(System::String^ fileName, unsigned int password)
		{
			if (AssetExists(fileName))
			{
				DataPack^ pack = Newtonsoft::Json::JsonConvert::DeserializeObject<DataPack^>(File::ReadAllText("Data/" + fileName + ".asset"));
				String^ serializedData = Newtonsoft::Json::JsonConvert::SerializeObject(this, Newtonsoft::Json::Formatting::Indented);
				String^ cipheredContents = Engine::Encryption::CypherLib::EncryptString(serializedData, password);

				CloneDataPack(pack);
				//System::IO::File::WriteAllText("Data/" + scene->sceneName + ".scn", System::Convert::ToBase64String(Encoding::UTF8->GetBytes(cipheredContents)));
			}
			else
			{
				// create file and rerun save
				printf("[CORE]: I/O OPERATION FAILED, FILE %s NOT FOUND", CastStringToNative(fileName).c_str());
				System::IO::File::Create("Data/" + fileName + ".asset")->Close();
				WriteToFile(fileName, password);

				ReadFromFile(fileName, password);
			}
		}

	public:
		void ReloadModel(unsigned int modelId)
		{
			String^ modelStr = this->models[modelId];
			AddModel(modelId, modelStr);
		}

		void ReloadTexture(unsigned int textureId)
		{
			String^ textureStr = this->textures2d[textureId];
			AddTextures2D(textureId, textureStr);
		}

		void ReloadShader(unsigned int shaderId)
		{
			cli::array<String^>^ str = gcnew cli::array<String^>(2);
			this->shaders->TryGetValue(shaderId, str);
			String^ vertexShader = str[0];
			String^ fragmentShader = str[1];
			AddShader(shaderId, vertexShader, fragmentShader);
		}

		void ReloadMaterial(unsigned int materialId)
		{
			String^ soundStr = this->materials[materialId];
			AddMaterial(materialId, soundStr);
		}

		void ReloadSound(unsigned int soundId)
		{
			String^ soundStr = this->sounds[soundId];
			AddSound(soundId, soundStr);
		}

		void ReloadMusic(unsigned int musicId)
		{
			String^ musicStr = this->musics[musicId];
			AddMusic(musicId, musicStr);
		}

		static DataPack^ singleton()
		{
			return singletonRef;
		}
	};

}