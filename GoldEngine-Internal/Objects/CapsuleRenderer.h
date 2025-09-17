#pragma once

namespace Engine::EngineObjects::Geometry
{
	public ref class CapsuleRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	public:
		[Engine::Scripting::PropertyAttribute] float Height = 2;
		[Engine::Scripting::PropertyAttribute] float Radius = 1;
		[Engine::Scripting::PropertyAttribute] int Slices = 16;
		[Engine::Scripting::PropertyAttribute] int Rings = 16;
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ Tint;

	public:
		CapsuleRenderer(String^ name, Engine::Internal::Components::Transform^ transform);

		void Start() override;
		void Draw() override;

		RAYLIB::Model& GetModel() override;
	};
}