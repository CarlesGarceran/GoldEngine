#include "../SDK.h"
#include "ModelAnimation.h"
#include "../Abstract/Renderer.h"
#include "../MeshRenderer/MeshRenderer.h"
#include "../ModelRenderer/ModelRenderer.hpp"
#include "../Time.h"


Engine::EngineObjects::Animation::AnimationLibraryEntry::AnimationLibraryEntry(unsigned int animId, unsigned int animIdx)
{
	this->animationId = animId;
	this->animationIndex = animIdx;
}

using Renderer = Engine::EngineObjects::Geometry::Abstract::Renderer;
using ModelRenderer = Engine::EngineObjects::Geometry::ModelRenderer;
using MeshRenderer = Engine::EngineObjects::Geometry::MeshRenderer;

Engine::EngineObjects::Animation::ModelAnimation::ModelAnimation()
{
	this->animationLibrary = gcnew Dictionary<String^, AnimationLibraryEntry>();
}

void Engine::EngineObjects::Animation::ModelAnimation::Awake()
{

}

void Engine::EngineObjects::Animation::ModelAnimation::Update()
{
	if (Parent == nullptr) return;
	if (!Parent->IsA<Renderer^>()) return;

	RAYLIB::Model& model = Parent->As<Renderer^>()->GetModel();

	AnimationStruct modelAnimation = DataPacks::singleton().GetAnimationStruct(animationId);
	animationIndex = System::Math::Clamp(
		animationIndex, 
		0u, 
		modelAnimation.animationCount-1
	);

	RAYLIB::ModelAnimation& anim = modelAnimation.animations[animationIndex];

	if (!RAYLIB::IsModelAnimationValid(model, anim) && validateBoneCount) return;

	if (!playing) return;

	currentFrame += Engine::Scripting::Time::deltaTime * PlaybackSpeed * 60.0f;
	
	if (currentFrame >= anim.keyframeCount)
	{
		OnFinishedPlaying->Invoke(gcnew cli::array<System::Object^>{ animationId });
		playing = false;
	}

	if (GPUSkinning)
	{
		RAYLIB::UpdateModelAnimationBones(
			model,
			anim,
			currentFrame
		);
	}
	else
	{
		RAYLIB::UpdateModelAnimation(
			model,
			anim,
			currentFrame
		);
	}
}

void Engine::EngineObjects::Animation::ModelAnimation::Destroy()
{
	OnFinishedPlaying->DisconnectAll();
}

void Engine::EngineObjects::Animation::ModelAnimation::Play()
{
	playing = true;
}

void Engine::EngineObjects::Animation::ModelAnimation::Play(unsigned int id)
{
	animationId = id;
	playing = true;
}

void Engine::EngineObjects::Animation::ModelAnimation::Play(String^ name)
{
	AnimationLibraryEntry entry = animationLibrary[name];
	animationId = entry.animationId;
	animationIndex = entry.animationIndex;
	playing = true;
}

void Engine::EngineObjects::Animation::ModelAnimation::Stop()
{
	playing = false;
	currentFrame = 0;
}

void Engine::EngineObjects::Animation::ModelAnimation::Pause()
{
	playing = false;
}

void Engine::EngineObjects::Animation::ModelAnimation::NextFrame()
{
	currentFrame++;
}

void Engine::EngineObjects::Animation::ModelAnimation::PreviousFrame()
{
	currentFrame--;
}

void Engine::EngineObjects::Animation::ModelAnimation::AddAnimation(String^ animationName, unsigned int animId, unsigned int animIndex)
{
	animationLibrary->Add(
		animationName,
		AnimationLibraryEntry(animId, animIndex)
	);
}

bool Engine::EngineObjects::Animation::ModelAnimation::IsPlaying::get()
{
	return playing;
}

void Engine::EngineObjects::Animation::ModelAnimation::IsPlaying::set(bool value)
{
	Stop();
	Play();
}

bool Engine::EngineObjects::Animation::ModelAnimation::IsPaused::get()
{
	return playing && currentFrame != 0;
}

void Engine::EngineObjects::Animation::ModelAnimation::IsPaused::set(bool value)
{
	Play();
}
