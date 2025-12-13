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

	/*
	isPlaying = false;
	wasPlaying = false;
	*/
}

void AudioSource::Update()
{
	if (&soundPtr->getInstance() == NULL)
		return;

	/*
	if (triggerPlay)  // SET DEBOUNCER, JUST IN CASE ATTRIBUTE FLAGS TRUE ON ISPLAYING
	{
		isPlaying = false;
		triggerPlay = false;
	}
	*/

	if (isPlaying)
	{
		if (resourceType == ResourceType::Music)
		{
			RAYLIB::SetMusicVolume(soundPtr->getInstance().music, volume);
			RAYLIB::SetMusicPan(soundPtr->getInstance().music, panning);
			RAYLIB::SetMusicPitch(soundPtr->getInstance().music, pitch);

			soundPtr->getInstance().music.looping = isLooped;

			UpdateMusicStream(soundPtr->getInstance().music);
		}
		else if (resourceType == ResourceType::Sound)
		{
			RAYLIB::SetAudioStreamVolume(soundPtr->getInstance().sound.stream, volume);
			RAYLIB::SetAudioStreamPan(soundPtr->getInstance().sound.stream, panning);
			RAYLIB::SetAudioStreamPitch(soundPtr->getInstance().sound.stream, pitch);
		}

		if (!wasPlaying)
		{
			if (resourceType == ResourceType::Sound && !IsSoundPlaying(soundPtr->getInstance().sound))
			{
				if (!IsSoundValid(soundPtr->getInstance().sound))
				{
					print("[AudioSource]", "Sound is INVALID, cannot play!");
				}

				RAYLIB::PlayAudioStream(soundPtr->getInstance().sound.stream);
			}
			else if (resourceType == ResourceType::Music && !IsMusicStreamPlaying(soundPtr->getInstance().music))
			{
				RAYLIB::PlayMusicStream(soundPtr->getInstance().music);
			}

			wasPlaying = true;
		}

		if (resourceType == ResourceType::Music)
		{
			if (RAYLIB::IsMusicStreamPlaying(soundPtr->getInstance().music) && !isLooped)
				isPlaying = false;
		}
		else if (resourceType == ResourceType::Sound)
		{
			if (RAYLIB::IsAudioStreamPlaying(soundPtr->getInstance().sound.stream) && !isLooped)
				isPlaying = false;
			else
				wasPlaying = false;
		}
	}
	else
	{
		if (wasPlaying)
		{
			if (resourceType == ResourceType::Sound)
				RAYLIB::StopAudioStream(soundPtr->getInstance().sound.stream);
			else if (resourceType == ResourceType::Music)
				RAYLIB::StopMusicStream(soundPtr->getInstance().music);

			wasPlaying = false;
		}
	}
}

void AudioSource::Destroy()
{
	if (wasPlaying)
	{
		if (resourceType == ResourceType::Sound)
			RAYLIB::StopAudioStream(soundPtr->getInstance().sound.stream);
		else if (resourceType == ResourceType::Music)
			RAYLIB::StopMusicStream(soundPtr->getInstance().music);

		wasPlaying = false;
	}

	delete soundPtr;
}

void AudioSource::Play()
{
	wasPlaying = false;
	isPlaying = true;
}

void AudioSource::Stop()
{
	wasPlaying = true;
	isPlaying = false;
}