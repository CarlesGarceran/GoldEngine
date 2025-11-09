#include "../SDK.h"
#include "Abstract/Renderer.h"
#include "CapsuleRenderer.h"

using namespace Engine::EngineObjects;

Engine::EngineObjects::Geometry::CapsuleRenderer::CapsuleRenderer(String^ name, Engine::Internal::Components::Transform^ transform)
	: Engine::EngineObjects::Geometry::Abstract::Renderer(name, transform),
	  Tint(Engine::Components::Color::New(0xFFFFFFFF))
{
}

void Engine::EngineObjects::Geometry::CapsuleRenderer::Start()
{
	if (Tint == nullptr) Tint = gcnew Engine::Components::Color(0xFFFFFFFF);

}

void Engine::EngineObjects::Geometry::CapsuleRenderer::Draw()
{
	RAYLIB::DrawCapsule(
		(transform->position - Engine::Components::Vector3(0, Height / 2, 0)).toNative(),
		(transform->position + Engine::Components::Vector3(0, Height / 2, 0)).toNative(),
		Radius,
		Slices,
		Rings,
		Tint->toNative()
	);
}

RAYLIB::Model* Engine::EngineObjects::Geometry::CapsuleRenderer::GetModel()
{
	return nullptr;
}
