#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class MeshRenderer : Engine::EngineObjects::Geometry::Abstract::Renderer
	{
		Engine::Native::EnginePtr<RAYLIB::Mesh>* meshInstance = nullptr;

	public:
		[Engine::Scripting::SerializePropertyAttribute] unsigned int modelId;
		[Engine::Scripting::SerializePropertyAttribute] unsigned int materialId;
		[Engine::Scripting::SerializePropertyAttribute] unsigned int meshIndex;

	public:
		MeshRenderer();

		void Awake() override;

		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;

		void Draw() override;
	
		void Destroy() override;

		RAYLIB::Model& GetModel() override;
		RAYLIB::Model* GetModelPtr() override;

		property Engine::Components::Material^ sharedMaterial { Engine::Components::Material^ get() override; }
		property cli::array<Engine::Components::Material^>^ sharedMaterials { cli::array<Engine::Components::Material^>^ get() override; }

	private:
		void onModelUpdated(unsigned int newId, unsigned int oldId);
		void onMeshIndexUpdated(unsigned int newId, unsigned int oldId);
	};
}