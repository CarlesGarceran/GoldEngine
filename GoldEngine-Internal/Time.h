#pragma once

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute("Time")]
	public ref class Time
	{
	private:
		static float _timeScale = 1.0f;

	public:
		static void Wait(double);

		static property int FPS
		{
			int get()
			{
				return RAYLIB::GetFPS();
			}
		}

		static property float timeScale
		{
			float get();
			void set(float value);
		}

		static property float deltaTime
		{
			float get();
		}

		static property double time
		{
			double get();
		}

		static property float unscaledDeltaTime 
		{
			float get();
		}

		static property double unscaledTime
		{
			double get();
		}

		static property float fixedDeltaTime
		{
			float get();
		}

		static property float unscaledFixedDeltaTime
		{
			float get();
		}
	};
};