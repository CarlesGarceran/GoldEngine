#pragma once

#include "Includes.h"
#include "CastToNative.h"
#include "DataPacks.h"
#include "CypherLib.h"
#include "Raylib/include/imgui_impl_raylib.h"

using namespace System;
using namespace System::Collections;
using namespace Engine::Assets::Storage;

namespace Engine
{
	public ref class Window
	{
	private:
		int tickRate = 0;
		int physicsTick = 60;
		int targetFPS = 60;
		int oldTargetFps;

		ArrayList drawList;

	public:
		bool FirstTimeBoot();
		void Boot();

		void SetWindowFlags(unsigned int flags)
		{
			SetConfigFlags(flags);
		}

		void OpenWindow(int width, int height, System::String^ name)
		{
			OpenWindow(width, height, CastToNative(name));
		}

		void OpenWindow(int width, int height, const char* name);

		void InitializeExtensions();

		void AddToDraw(System::Action^ invokable)
		{
			drawList.Add(invokable);
		}

		void ResetPhysics()
		{
			tickRate = 0;
		}

		void SetFPS(int fps)
		{
			oldTargetFps = targetFPS;
			targetFPS = fps;
		}

		int GetFPS()
		{
			return targetFPS;
		}

		void Loop()
		{
			while (!RAYLIB::WindowShouldClose())
			{
				if (oldTargetFps != targetFPS)
				{
					SetTargetFPS(targetFPS);
					oldTargetFps = targetFPS;
				}

				Update();
				Draw();
			}

			rlImGuiShutdown();
			ImNodes::DestroyContext();
			Exit();
			RAYLIB::CloseAudioDevice();
			RAYLIB::CloseWindow();
		}

		// stubmethods

		virtual System::Object^ GetSelectedObject() { return nullptr; }

		// vmethods
		virtual void Preload() {} // loading assets
		virtual void Start() {}
		virtual void Init() {} // Initialization (spawning objects, scene loading, whatever.)
		virtual void Exit() {} // exit
		virtual void Draw() {} // draw
		virtual void Update() {} // update 
		virtual void DrawImGui() {}
	};
}