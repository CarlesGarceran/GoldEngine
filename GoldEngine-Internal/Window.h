#pragma once

#include "Includes.h"
#include "CastToNative.h"
#include "DataPacks.h"
#include "CypherLib.h"

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

		ArrayList drawList;

	public:
		bool FirstTimeBoot()
		{
			Singleton<Window^>::Create(this);
			HarmonyLib::Harmony^ harmony = gcnew HarmonyLib::Harmony("HarmonyInstance");
			Singleton<HarmonyLib::Harmony^>::Create(harmony);

			return !System::IO::File::Exists("./Data/UserData/firstInit.asset");
		}

		void Boot()
		{
			System::IO::File::Create("./Data/UserData/firstInit.asset")->Close();
		}

		void SetWindowFlags(unsigned int flags)
		{
			SetConfigFlags(flags);
		}

		void OpenWindow(int width, int height, System::String^ name)
		{
			OpenWindow(width, height, CastToNative(name));
		}

		void OpenWindow(int width, int height, const char* name)
		{
			EngineState::glInitialized = true;

			InitWindow(width, height, name);
			InitAudioDevice();
			RLGL::rlglInit(width, height);

			RLGL::rlEnableDepthTest();

			InitializeExtensions();

			rlImGuiSetup(true);
			ImNodes::CreateContext();
		}

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
			targetFPS = fps;
		}

		int GetFPS()
		{
			return targetFPS;
		}

		void Loop()
		{
			SetTargetFPS(targetFPS);

			while (!WindowShouldClose())
			{
				Update();
				Draw();
			}

			rlImGuiShutdown();
			ImNodes::DestroyContext();
			Exit();
			CloseAudioDevice();
			CloseWindow();
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