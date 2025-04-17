#pragma once

#include "Includes.h"
#include "ManagedIncludes.h"
#include "EngineState.h"
#include "GlIncludes.h"

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute("Screen")]
	public ref class Screen
	{
	private:
		static int width;
		static int height;
		static int x;
		static int y;

	public:
		static property int Width
		{
#ifdef PRODUCTION_BUILD
			int get()
			{
				return RAYLIB::GetScreenWidth();
			}
#else
			int get()
			{
				if (EngineState::PlayMode)
				{
					return RAYLIB::GetScreenWidth();
				}

				return width;
			}
#endif
		}

	public:
		static property int Height
		{
#ifdef PRODUCTION_BUILD
			int get()
			{
				return RAYLIB::GetScreenHeight();
			}
#else
			int get()
			{
				if (EngineState::PlayMode)
				{
					return RAYLIB::GetScreenHeight();
				}

				return height;
			}
#endif
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
	};
}