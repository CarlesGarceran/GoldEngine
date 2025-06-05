#pragma once

#include <stdarg.h>

using namespace Engine::Assets::Storage;

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class ModelRenderer : public Engine::Internal::Components::GameObject
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* modelInstance;
		Engine::Native::EnginePtr<RAYLIB::Shader>* shaderInstance;
		Engine::Native::EnginePtr<RAYLIB::Texture2D>* textureInstance;

		void setup()
		{
			RAYLIB::Color c =
			{
				(tint >> 0) & 0xFF,
				(tint >> 8) & 0xFF,
				(tint >> 16) & 0xFF,
				(tint >> 24) & 0xFF
			};


			RAYLIB::Model& model = modelInstance->getInstance();

			model.materials[0].shader = shaderInstance->getInstance();

			model.materials[0].maps[MATERIAL_MAP_ALBEDO].color = c;
			model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = (Texture2D)textureInstance->getInstance();

			model.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.5f;
			model.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
			model.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 0.0f;
			model.materials[0].maps[MATERIAL_MAP_EMISSION].color = RAYLIB::WHITE;
			model.materials[0].maps[MATERIAL_MAP_EMISSION].value = 0.0f;

			RLGL::rlCheckErrors();
		}

	public:
		unsigned int model;
		unsigned int shader;
		unsigned int texture;
		unsigned int tint;
		
	private:
		Engine::Components::Material^ material;

	public:

		[Newtonsoft::Json::JsonConstructorAttribute]
		ModelRenderer(String^ name, Engine::Internal::Components::Transform^ trans, unsigned int model, unsigned int shader, unsigned int texture, unsigned int tint) : Engine::Internal::Components::GameObject(name, trans, Engine::Internal::Components::ObjectType::ModelRenderer, this->tag, Engine::Scripting::LayerManager::GetLayerFromId(1))
		{
			this->model = model;
			this->shader = shader;
			this->tint = tint;
			this->texture = texture;

			/*
			this->material = gcnew Reflectable::Generic::Reflectable<Engine::Components::Material^>(gcnew Engine::Components::Material());
			this->material->SetMainTexture(texture);
			*/

			this->modelInstance = nullptr;
			this->shaderInstance = nullptr;
			this->textureInstance = nullptr;
		}

		void Init(unsigned int model, unsigned int shader, unsigned int texture, unsigned int tint)
		{
			this->model = model;
			this->shader = shader;
			this->tint = tint;
			this->texture = texture;

			this->modelInstance = nullptr;
			this->shaderInstance = nullptr;
			this->textureInstance = nullptr;
			/*
			modelInst.materials[0].shader = DataPacks::singleton().GetShader(shader);
			modelInst.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.0f;
			modelInst.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
			modelInst.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
			modelInst.materials[0].maps[MATERIAL_MAP_EMISSION].color = WHITE;
			modelInst.materials[0].maps[MATERIAL_MAP_EMISSION].value = 0.0f;
			*/

		}

		void Update() override 
		{

		}

		void PhysicsUpdate() override {}
		void Start() override {

			/*
			modelManager = new Native::NativeModel(
				DataPacks::singleton().GetModel(model),
				DataPacks::singleton().GetTexture2D(texture),
				DataPacks::singleton().GetShader(shader),
				tint
			);
			*/
		}

		void Draw() override
		{
			if (modelInstance != nullptr)
				delete modelInstance;

			if (shaderInstance != nullptr)
				delete shaderInstance;

			if (textureInstance != nullptr)
				delete textureInstance;

			RAYLIB::Model& rl_model = DataPacks::singleton().GetModel(model);
			RAYLIB::Shader& rl_shader = DataPacks::singleton().GetShader(shader);
			RAYLIB::Texture2D& rl_texture = DataPacks::singleton().GetTexture2D(texture);

			this->modelInstance = new Engine::Native::EnginePtr<RAYLIB::Model>(rl_model);
			this->shaderInstance = new Engine::Native::EnginePtr<RAYLIB::Shader>(rl_shader);
			this->textureInstance = new Engine::Native::EnginePtr<RAYLIB::Texture2D>(rl_texture);

			if (!modelInstance->isLoaded())
				modelInstance->setInstance(DataPacks::singleton().GetModel(model));

			if (!shaderInstance->isLoaded())
				shaderInstance->setInstance(DataPacks::singleton().GetShader(shader));

			if (!textureInstance->isLoaded())
				textureInstance->setInstance(DataPacks::singleton().GetTexture2D(texture));

			auto t = getTransform();

			RAYLIB::Color c =
			{
					(tint >> 0) & 0xFF,
					(tint >> 8) & 0xFF,
					(tint >> 16) & 0xFF,
					(tint >> 24) & 0xFF
			};


			RAYLIB::Vector2 baseVctr = { 0.5f, 0.5f };
			RAYLIB::Vector4 EmissiveColor = ColorNormalize(modelInstance->getInstance().materials[0].maps[MATERIAL_MAP_EMISSION].color);

			int emmisiveIntensityLocation = GetShaderLocation(shaderInstance->getInstance(), "emissivePower");
			int emmisiveColorLocation = GetShaderLocation(shaderInstance->getInstance(), "emissiveColor");
			int textureTilingLocation = GetShaderLocation(shaderInstance->getInstance(), "tiling");
			float emissiveIntensity = 0.01f;

			RAYLIB::SetShaderValue(shaderInstance->getInstance(), textureTilingLocation, &baseVctr, SHADER_UNIFORM_VEC2);
			RAYLIB::SetShaderValue(shaderInstance->getInstance(), emmisiveColorLocation, &EmissiveColor, SHADER_UNIFORM_VEC4);
			RAYLIB::SetShaderValue(shaderInstance->getInstance(), emmisiveIntensityLocation, &emissiveIntensity, SHADER_UNIFORM_FLOAT);

			RLGL::rlCheckErrors();

			setup();

			Model& m = modelInstance->getInstance();

			m.transform = RAYMATH::MatrixRotateXYZ({
				DEG2RAD*this->transform->rotation->x,
				DEG2RAD* this->transform->rotation->y,
				DEG2RAD* this->transform->rotation->z
			});

			DrawModelEx(
				m,
				{ t->position->x,t->position->y, t->position->z },
				{0,0,0},
				0.0f,
				t->scale->toNative(),
				c
			);
		}

		void SetModel(unsigned int m)
		{
			this->model = m;
		}

		void SetMaterial(unsigned int mat)
		{
			auto modelInst = Engine::Assets::Storage::DataPacks::singleton().GetModel(model);
			auto materialInst = DataPacks::singleton().GetShader(shader);
			auto texInst = DataPacks::singleton().GetTexture2D(texture);

			modelInst.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texInst;
		}

		void SetColorTint(unsigned int hexValue)
		{
			tint = hexValue;
		}

		void Destroy() override
		{
			GameObject::Destroy();
		}
};
}