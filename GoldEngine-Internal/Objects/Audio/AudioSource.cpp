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

bool triggerPlay = true;

void AudioSource::onSoundChanged(unsigned int newId, unsigned int oldId)
{
	Stop();

	AudioData data = AudioData(
		DataPacks::singleton().GetSound(soundId),
		DataPacks::singleton().GetMusic(soundId)
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

void AudioSource::Start()
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

    if (isPlaying)
    {
        if (resourceType == ResourceType::Music)
        {
            RAYLIB::SetMusicVolume(audio.music, volume);
            RAYLIB::SetMusicPan(audio.music, panning);
            RAYLIB::SetMusicPitch(audio.music, pitch);

            audio.music.looping = isLooped;
            UpdateMusicStream(audio.music);

            if (!wasPlaying && !IsMusicStreamPlaying(audio.music))
            {
                RAYLIB::PlayMusicStream(audio.music);
                wasPlaying = true;
            }

            if (!RAYLIB::IsMusicStreamPlaying(audio.music) && !isLooped)
                isPlaying = false;
        }
        else // ResourceType::Sound
        {
            RAYLIB::SetAudioStreamVolume(audio.sound.stream, volume);
            RAYLIB::SetAudioStreamPan(audio.sound.stream, panning);
            RAYLIB::SetAudioStreamPitch(audio.sound.stream, pitch);

            if (!wasPlaying && !IsSoundPlaying(audio.sound))
            {
                if (!IsSoundValid(audio.sound))
                    print("[AudioSource]", "Sound is INVALID, cannot play!");

                RAYLIB::PlayAudioStream(audio.sound.stream);
                wasPlaying = true;
            }

            if (!RAYLIB::IsAudioStreamPlaying(audio.sound.stream) && !isLooped)
                isPlaying = false;
        }
    }
    else if (wasPlaying)
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