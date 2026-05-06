#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	public:
		[Engine::Scripting::SerializePropertyAttribute] unsigned int modelId;
		[Engine::Scripting::SerializePropertyAttribute] unsigned int materialId;
	public:
		ModelRenderer();

		void Awake() override;
		void Draw() override;

		void Destroy() override;

		RAYLIB::Model& GetModel() override;
		RAYLIB::Model* GetModelPtr() override;

		property Engine::Components::Material^ sharedMaterial { Engine::Components::Material^ get() override; }
		property cli::array<Engine::Components::Material^>^ sharedMaterials { cli::array<Engine::Components::Material^>^ get() override; }

	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* modelPtr;

		void OnModelIdChanged(unsigned int newId, unsigned int oldId);
	};
}