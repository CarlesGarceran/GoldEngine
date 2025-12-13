#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* model;

	public:
		[Engine::Scripting::SerializePropertyAttribute] unsigned int modelId;
		[Engine::Scripting::SerializePropertyAttribute] unsigned int materialId;

		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ tint;

	public:
		ModelRenderer();

		void Setup() override;

		void Draw() override;

		void Destroy() override;

		RAYLIB::Model* GetModel() override;

	private:
		void onModelUpdated(unsigned int newId, unsigned int oldId);
	};
}