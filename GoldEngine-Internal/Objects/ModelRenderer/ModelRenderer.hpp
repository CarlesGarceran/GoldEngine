#pragma once

namespace Engine::EngineObjects::Geometry
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	public:
		[Newtonsoft::Json::JsonIgnoreAttribute] [Engine::Scripting::SerializePropertyAttribute] unsigned int modelId;
		[Newtonsoft::Json::JsonIgnoreAttribute] [Engine::Scripting::SerializePropertyAttribute] unsigned int materialId;
		[Newtonsoft::Json::JsonIgnoreAttribute] [Engine::Scripting::SerializePropertyAttribute] Engine::Components::Color^ tint;
	
	public:
		ModelRenderer();

		void Awake() override;
		void Draw() override;

		void Destroy() override;

		RAYLIB::Model& GetModel() override;
		RAYLIB::Model* GetModelPtr() override;

	private:
		Engine::Native::EnginePtr<RAYLIB::Model>* modelPtr;

		void OnModelIdChanged(unsigned int newId, unsigned int oldId);
	};
}