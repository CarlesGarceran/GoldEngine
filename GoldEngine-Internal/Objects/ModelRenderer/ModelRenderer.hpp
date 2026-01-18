#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	public:
		[Engine::Scripting::PropertyAttribute] unsigned int modelId;
		[Engine::Scripting::PropertyAttribute] unsigned int materialId;

		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ tint;

	public:
		ModelRenderer();

		void Awake() override;
		void Draw() override;

		void Destroy() override;

		RAYLIB::Model* GetModel() override;
	};
}