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
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int soundId;
		[Engine::Scripting::PropertyAttribute]
		float volume = 1.0f;
		[Engine::Scripting::PropertyAttribute]
		float panning = 0.5f;
		[Engine::Scripting::PropertyAttribute]
		float pitch = 1.0f;
		[Engine::Scripting::PropertyAttribute]
		PanType panType = PanType::Pan2D; // HARDLOCK READONLY AS THERE'S NO PANNING ALGORITHM YET.
		[Engine::Scripting::PropertyAttribute]
		ResourceType resourceType = ResourceType::Sound;
		[Engine::Scripting::PropertyAttribute]
		bool isLooped = true;
		[Engine::Scripting::PropertyAttribute]
		bool isPlaying = false;
	
	private:
		Engine::Native::EnginePtr<AudioData>* soundPtr;
		bool wasPlaying = false;

	public:
		void Awake() override;

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;
		void Destroy() override;

		void Play();
		void Stop();

	private:
		void onSoundChanged(unsigned int, unsigned int);
		void setupAudioStream(RAYLIB::Sound sound);
		void setupMusicStream(RAYLIB::Music music);

	public:
		RAYLIB::AudioStream GetAudioStream();
	};
}