#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelRenderer : Engine::EngineObjects::Script
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* model;
		Engine::Components::Material^ material = nullptr;

	public:
		[Engine::Scripting::PropertyAttribute] unsigned int modelId;
		[Engine::Scripting::PropertyAttribute] unsigned int materialId;

		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ tint;

	public:
		ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform);
		ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId);
		ModelRenderer(String^ name, Engine::Internal::Components::Transform^ transform, unsigned int modelId, unsigned int materialId, Engine::Components::Color^ tint);

		void Start() override;

		void Draw() override;

		void Destroy() override;

	private:
		void onModelUpdated(unsigned int newId, unsigned int oldId);
		void onMaterialUpdated(unsigned int newId, unsigned int oldId);
	};
}