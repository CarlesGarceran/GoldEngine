#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
		public ref class RenderTexture
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::RenderTexture>* resource;

	public:
		property int Width { int get(); }
		property int Height { int get(); }

		RenderTexture(int width, int height);
		RenderTexture(int width, int height, bool attachDepth);
		~RenderTexture();

		property RAYLIB::RenderTexture* Handle{ RAYLIB::RenderTexture* get(); }

	internal:
		RAYLIB::RenderTexture& GetRenderTexture();
		RAYLIB::RenderTexture* GetRenderTexturePointer();
	};
}