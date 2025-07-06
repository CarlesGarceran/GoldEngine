#include "../../SDK.h"
#include "AudioSource.h"

using namespace Engine::EngineObjects;

#pragma managed(push, off)
void onUnloaded(AudioData sound)
{
	// do nothing lol, no need to unload resources (unmanaged btw, we're working with native structs)

	return;
}
#pragma managed(pop)

bool triggerPlay = true;

AudioSource::AudioSource(String^ name, Engine::Internal::Components::Transform^ transform)
	: Script(name, transform)
{
}

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
			RAYLIB::SetSoundVolume(soundPtr->getInstance().sound, volume);
			RAYLIB::SetSoundPan(soundPtr->getInstance().sound, panning);
			RAYLIB::SetSoundPitch(soundPtr->getInstance().sound, pitch);
		}

		if (!wasPlaying)
		{
			if (resourceType == ResourceType::Sound)
			{
				RAYLIB::PlaySound(soundPtr->getInstance().sound);
			}
			else if (resourceType == ResourceType::Music)
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
			if (RAYLIB::IsSoundPlaying(soundPtr->getInstance().sound) && !isLooped)
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
				RAYLIB::StopSound(soundPtr->getInstance().sound);
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
			RAYLIB::StopSound(soundPtr->getInstance().sound);
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