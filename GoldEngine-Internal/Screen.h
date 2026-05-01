#pragma once

#include "Includes.h"
#include "ManagedIncludes.h"
#include "EngineState.h"
#include "GlIncludes.h"
#include "Object/Vector2.h"

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute("Screen")]
	public ref class Screen
	{
	private:
		static int width = RAYLIB::GetScreenWidth();
		static int height = RAYLIB::GetScreenHeight();
		static int x = 0;
		static int y = 0;
		static bool useVirtualResolution = false;

	public:
		static property int ScreenWidth
		{
			int get() { return RAYLIB::GetScreenWidth(); }
		}
		static property int ScreenHeight
		{
			int get() { return RAYLIB::GetScreenHeight(); }
		}

		static property int RenderWidth
		{
			int get() { return width; }
		}
		static property int RenderHeight
		{
			int get() { return height; }
		}

		static property int Width
		{
			int get();
		}

	public:
		static property int Height
		{
			int get();
		}

	public:
		static int getX()
		{
			return Screen::x;
		}

		static int getY()
		{
			return Screen::y;
		}

		static void setX(const int& x)
		{
			Screen::x = x;
		}

		static void setY(const int& y)
		{
			Screen::y = y;
		}

		static void setWidth(const int& width)
		{
			Screen::width = width;
		}

		static void setHeight(const int& height)
		{
			Screen::height = height;
		}

		static void UseRenderResolution(bool value);
		static void SetRenderWidth(int width);
		static void SetRenderHeight(int height);

		static void SetRenderScale(Engine::Components::Vector2 scale);

		static Engine::Components::Vector2 GetRenderScale();
		static int GetRenderWidth();
		static int GetRenderHeight();

		static bool IsVirtualResolutionInUse();
	};
}