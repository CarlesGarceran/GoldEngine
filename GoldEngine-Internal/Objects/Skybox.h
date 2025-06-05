#pragma once

namespace Engine::EngineObjects
{
	namespace Native
	{
		class NativeSkybox
		{
		public:
			std::unique_ptr<Model> model;
			NativeSkybox(Model newMod)
			{
				model = std::make_unique<Model>(newMod);
			}
		};
	}

	[System::ObsoleteAttribute("The class Skybox is obsolete, please, use the ModelRenderer for this. (This class may be deleted or used for internal testing purposes and might have unexpected behaviours)")]
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Skybox : Engine::Internal::Components::GameObject
	{
	public:
		unsigned int model;
		unsigned int material;
		unsigned int texture;

		Skybox(System::String^ s, Engine::Internal::Components::Transform^ t, unsigned int modelId, unsigned int matId, unsigned int texturePath) : GameObject(s, t, Engine::Internal::Components::ObjectType::Skybox, this->tag, Engine::Scripting::LayerManager::GetLayerFromId(1))
		{
			material = matId;
			texture = texturePath;
			model = modelId;
		}

		void Init(unsigned int modelId, unsigned int matId, unsigned int texturePath) override;

		void Draw() override;

		void DrawGizmo() override
		{

		}

		void Update() override
		{

		}

		void PhysicsUpdate() override
		{

		}

		void Start() override
		{

		}

		void SetupSkyboxImage()
		{

		}

		void SwapMaterial(unsigned int materialId)
		{
			
		}
	};
}