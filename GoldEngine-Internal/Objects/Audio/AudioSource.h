#pragma once

namespace Engine::EngineObjects
{
	struct AudioData
	{
	public:
		RAYLIB::Sound sound;
		RAYLIB::Music music;

	public:
		AudioData() 
		{
			this->sound = { 0 };
			this->music = { 0 };
		}

		AudioData(Sound s, Music m)
		{
			this->sound = s;
			this->music = m;
		}
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class AudioSource : Script
	{
	public:
		enum class PanType
		{
			Pan2D,
			Pan3D
		};

		enum class ResourceType
		{
			Music,
			Sound
		};

	public:
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int soundId;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		float volume = 1.0f;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		float panning = 0.5f;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		float pitch = 1.0f;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::ReadOnly)]
		PanType panType = PanType::Pan2D; // HARDLOCK READONLY AS THERE'S NO PANNING ALGORITHM YET.
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		ResourceType resourceType = ResourceType::Sound;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		bool isLooped = true;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		bool isPlaying = false;
	
	private:
		Engine::Native::EnginePtr<AudioData>* soundPtr;
		bool wasPlaying = false;

	public:
		AudioSource(String^ name, Engine::Internal::Components::Transform^ transform);

		void Start() override;

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;
		void Destroy() override;

		void Play();
		void Stop();

	private:
		void onSoundChanged(unsigned int, unsigned int);

	public:
		RAYLIB::AudioStream GetAudioStream();
	};
}