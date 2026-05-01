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
		[Engine::Scripting::SerializePropertyAttribute] unsigned int meshIndex;

		[Engine::Scripting::SerializePropertyAttribute] unsigned int materialId;
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ tint;

	public:
		MeshRenderer();

		void Awake() override;

		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;

		void Draw() override;
	
		void Destroy() override;

		RAYLIB::Model& GetModel() override;
		RAYLIB::Model* GetModelPtr() override;

	private:
		void onModelUpdated(unsigned int newId, unsigned int oldId);
		void onMeshIndexUpdated(unsigned int newId, unsigned int oldId);
	};
}