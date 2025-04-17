#pragma once

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute("Time")]
	public ref class Time
	{
	public:
		static void Wait(double);

		static property int FPS
		{
			int get()
			{
				return RAYLIB::GetFPS();
			}
		}

		static property float deltaTime
		{
			float get()
			{
				return RAYLIB::GetFrameTime();
			}
		}

		static property double time
		{
			double get()
			{
				return RAYLIB::GetTime();
			}
		}
	};
};