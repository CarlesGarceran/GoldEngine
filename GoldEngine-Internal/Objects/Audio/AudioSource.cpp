#include "../../SDK.h"
#include "AudioSource.h"

using namespace Engine::EngineObjects;

#pragma managed(push, off)
void onUnloaded(AudioData sound)
{
	// do nothing lol, the resources are comming from the datapacks, no need to free them unless we want to corrupt the game.
	sound.music = {};
	sound.sound = {};

	return;
}
#pragma managed(pop)

void AudioSource::setupAudioStream(RAYLIB::Sound sound)
{
    RAYLIB::SetAudioStreamVolume(sound.stream, volume);
    RAYLIB::SetAudioStreamPan(sound.stream, panning);
    RAYLIB::SetAudioStreamPitch(sound.stream, pitch);

    if (!wasPlaying && !IsAudioStreamPlaying(sound.stream))
    {
        if (!IsAudioStreamValid(sound.stream))
            print("[AudioSource]", "Sound is INVALID, cannot play!");

        RAYLIB::PlayAudioStream(sound.stream);
        wasPlaying = true;
    }
}

void Engine::EngineObjects::AudioSource::setupMusicStream(RAYLIB::Music music)
{
    RAYLIB::SetMusicVolume(music, volume);
    RAYLIB::SetMusicPan(music, panning);
    RAYLIB::SetMusicPitch(music, pitch);

    music.looping = isLooped;
    UpdateMusicStream(music);

    if (!wasPlaying && !IsMusicStreamPlaying(music))
    {
        RAYLIB::PlayMusicStream(music);
        wasPlaying = true;
    }
}

void AudioSource::onSoundChanged(unsigned int newId, unsigned int oldId)
{
    if (newId == oldId) return;

	Stop();

	AudioData data = AudioData(
		DataPacks::singleton().GetSound(newId),
		DataPacks::singleton().GetMusic(newId)
	);

	soundPtr->setInstanceRef(
		data
	);
}

RAYLIB::AudioStream Engine::EngineObjects::AudioSource::GetAudioStream()
{
	if (resourceType == ResourceType::Music)
		return soundPtr->getInstance().music.stream;

	if (resourceType == ResourceType::Sound)
		return soundPtr->getInstance().sound.stream;


	return { };
}

void AudioSource::Awake()
{
	AudioData data = AudioData(
		DataPacks::singleton().GetSound(soundId),
		DataPacks::singleton().GetMusic(soundId)
	);

	soundPtr = new Engine::Native::EnginePtr<AudioData>(data, &onUnloaded);

	if (attributes->hasAttribute("soundId"))
		attributes->getAttribute("soundId")->onPropertyChanged->connect(gcnew Action<unsigned int, unsigned int>(this, &AudioSource::onSoundChanged));
}

void AudioSource::Update()
{
    if (soundPtr == nullptr) return;

    AudioData& audio = soundPtr->getInstance();

    if (isPlaying && !wasPlaying)
    {
        setupAudioStream(audio.sound);
        return;
    }

    if (wasPlaying)
    {
        if (!isLooped)
        {
            if (resourceType == ResourceType::Music)
            {
                if (!RAYLIB::IsMusicStreamPlaying(audio.music))
                    isPlaying = false;
            }
            else // ResourceType::Sound
            {
                if (!RAYLIB::IsAudioStreamPlaying(audio.sound.stream))
                    isPlaying = false;
            }
        }
        else
        {
            if (!RAYLIB::IsAudioStreamPlaying(audio.sound.stream)) Play();
            if (!RAYLIB::IsMusicStreamPlaying(audio.music)) Play();
        }
    }

    if (!isPlaying)
    {
        if (resourceType == ResourceType::Sound)
            RAYLIB::StopAudioStream(audio.sound.stream);
        else
            RAYLIB::StopMusicStream(audio.music);

        wasPlaying = false;
    }
}

void AudioSource::Destroy()
{
    if (wasPlaying)
    {
        if (resourceType == ResourceType::Sound)
            RAYLIB::StopAudioStream(soundPtr->getInstance().sound.stream);
        else
            RAYLIB::StopMusicStream(soundPtr->getInstance().music);

        wasPlaying = false;
    }

    if (attributes != nullptr && attributes->hasAttribute("soundId"))
    {
        auto attr = attributes->getAttribute("soundId");
        if (attr != nullptr && attr->onPropertyChanged != nullptr)
            attr->onPropertyChanged->DisconnectAll();
    }

    delete soundPtr;
    soundPtr = nullptr;
}

void AudioSource::Play()
{
	isPlaying = true;
	wasPlaying = false;
}

void AudioSource::Stop()
{
	isPlaying = false;
}