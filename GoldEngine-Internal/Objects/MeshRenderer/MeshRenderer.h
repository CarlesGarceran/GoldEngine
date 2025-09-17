#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
		public ref class MeshRenderer : Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* modelInstance = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Mesh>* meshInstance = nullptr;
		Engine::Components::Material^ materialInstance = nullptr;

	public:
		[Engine::Scripting::PropertyAttribute] unsigned int modelId;
		[Engine::Scripting::PropertyAttribute] unsigned int meshIndex;

		[Engine::Scripting::PropertyAttribute] unsigned int materialId;
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ tint;

	public:
		MeshRenderer(String^ name, Engine::Internal::Components::Transform^ transform);
		MeshRenderer();

		void Start() override;

		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;

		void Draw() override;
	
		void Destroy() override;


		RAYLIB::Model& GetModel() override;
	};
}