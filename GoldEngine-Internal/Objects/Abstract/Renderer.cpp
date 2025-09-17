#include "../../SDK.h"
#include "Renderer.h"

Engine::EngineObjects::Geometry::Abstract::Renderer::Renderer()
	: Engine::EngineObjects::Script()
{

}

Engine::EngineObjects::Geometry::Abstract::Renderer::Renderer(String^ name, Engine::Internal::Components::Transform^ transform)
	: Engine::EngineObjects::Script(name, transform)
{

}
