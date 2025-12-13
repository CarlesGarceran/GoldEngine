#include "../../SDK.h"
#include "ParticleEmitter.h"

#include "../Abstract/Renderer.h"
#include "../MeshRenderer/MeshRenderer.h"

using namespace Engine::EngineObjects::Particles::Enums;
using namespace Engine::Components;

Engine::Components::Vector3 GetDirectionVector(EmissionDirection dir)
{
    switch (dir)
    {
    case EmissionDirection::Back: return Engine::Components::Vector3(0, 0, -1);
    case EmissionDirection::Bottom: return Engine::Components::Vector3(0, -1, 0);
    case EmissionDirection::Front: return Engine::Components::Vector3(0, 0, 1);
    case EmissionDirection::Left: return Engine::Components::Vector3(-1, 0, 0);
    case EmissionDirection::Right: return Engine::Components::Vector3(1, 0, 0);
    case EmissionDirection::Top: return Engine::Components::Vector3(0, 1, 0);
    }

    return Engine::Components::Vector3(0, 1, 0);
}

void Engine::EngineObjects::Particles::ParticleEmitter::Start()
{

}

void Engine::EngineObjects::Particles::ParticleEmitter::Update()
{
	if (Parent == nullptr)
	{
		if (shape == Enums::EmitterShape::Mesh)
		{
			printError("For using a mesh emitter shape, the parent of the emitter must be a mesh renderer");
			shape = Enums::EmitterShape::Box;
		}
		return;
	}

    if (shape == Enums::EmitterShape::Mesh)
    {
        if (!Parent->IsA<Engine::EngineObjects::Geometry::MeshRenderer^>())
        {
            printError("Emitter shape set to mesh, but parent is not a MeshRenderer. Switching to Box shape.");
            shape = Enums::EmitterShape::Box;
            return;
        }

		Engine::EngineObjects::Geometry::MeshRenderer^ meshRenderer = Parent->As<Engine::EngineObjects::Geometry::MeshRenderer^>();
    }

}
