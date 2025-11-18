#pragma once

namespace Engine::EngineObjects::Surface
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class RenderSurface : Engine::EngineObjects::Script 
	{ 
	public:
		RenderSurface(String^ name, Engine::Internal::Components::Transform^ transform);
		RenderSurface();
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class RenderSurface3D : RenderSurface
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::RenderTexture2D>* texturePtr;
		Engine::Native::EnginePtr<RAYLIB::Material>* material;
		Engine::Native::EnginePtr<RAYLIB::Model>* model;
		Engine::Native::EnginePtr<RAYLIB::RenderTexture2D>* viewport;

		bool failsafe = false;

	public:
		[Engine::Scripting::SerializePropertyAttribute]
		Engine::Components::Vector2 viewportSize;

		[Engine::Scripting::PropertyAttribute]
		Engine::Components::Color^ tintColor;

	public:
		RenderSurface3D(String^ name, Engine::Internal::Components::Transform^ transform);
		RenderSurface3D();

		void Start() override;

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;

		void Draw() override;

		void Destroy() override;

	private:
		void onViewportSizeChanged(Engine::Components::Vector2, Engine::Components::Vector2);
	};
}

