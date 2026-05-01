#pragma once

namespace Engine::EngineObjects::Animation
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public value struct AnimationLibraryEntry
	{
	public:
		unsigned int animationId, animationIndex;
		
		AnimationLibraryEntry(unsigned int animId, unsigned int animIdx);
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class ModelAnimation : public Engine::EngineObjects::Script
	{
	public:
		[Engine::Scripting::PropertyAttribute] bool validateBoneCount = true;
		[Engine::Scripting::PropertyAttribute] unsigned int animationId = 0;
		[Engine::Scripting::PropertyAttribute] unsigned int animationIndex = 0;
		[Newtonsoft::Json::JsonIgnoreAttribute] Engine::Scripting::Events::Event^ OnFinishedPlaying = Engine::Scripting::Events::Event::New();

		[Engine::Scripting::PropertyAttribute] float PlaybackSpeed = 1;
		[Engine::Scripting::PropertyAttribute] bool GPUSkinning = false;

		[Newtonsoft::Json::JsonIgnoreAttribute] 
		[Engine::Scripting::PropertyAttribute] 
		property bool IsPlaying {
			bool get();
			void set(bool value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[Engine::Scripting::PropertyAttribute]
		property bool IsPaused {
			bool get();
			void set(bool value);
		}

	private:
		[Newtonsoft::Json::JsonPropertyAttribute] Dictionary<String^, AnimationLibraryEntry>^ animationLibrary;

		bool playing;
		[Engine::Scripting::PropertyAttribute] float currentFrame;

	public:
		ModelAnimation();

		void Awake() override;
		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;

		void Destroy() override;

		void Play();
		void Play(unsigned int id);
		void Play(String^ name);


		void Stop();
		void Pause();

		void NextFrame();
		void PreviousFrame();

		void AddAnimation(String^ animationName, unsigned int animId, unsigned int animIndex);
	};
}