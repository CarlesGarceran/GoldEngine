#include "Includes.h"
#include "ManagedIncludes.h"
#include "GlIncludes.h"
#include "EngineState.h"
#include "Screen.h"

using namespace Engine::Scripting;

#ifdef PRODUCTION_BUILD
int Engine::Scripting::Screen::Height::get()
{
	if (useVirtualResolution)
	{
		return height;
	}

	return RAYLIB::GetScreenHeight();
}
#else
int Engine::Scripting::Screen::Height::get()
{
	if (useVirtualResolution)
	{
		return height;
	}

	if (EngineState::PlayMode)
	{
		return RAYLIB::GetScreenHeight();
	}

	return height;
}
#endif

#ifdef PRODUCTION_BUILD
int Engine::Scripting::Screen::Width::get()
{
	if (useVirtualResolution)
	{
		return width;
	}

	return RAYLIB::GetScreenWidth();
}
#else
int Engine::Scripting::Screen::Width::get()
{
	if (useVirtualResolution)
	{
		return width;
	}

	if (EngineState::PlayMode)
	{
		return RAYLIB::GetScreenWidth();
	}

	return width;
}
#endif

void Engine::Scripting::Screen::UseRenderResolution(bool value)
{
	useVirtualResolution = value;
}

void Engine::Scripting::Screen::SetRenderWidth(int width)
{
	Screen::width = width;
}

void Engine::Scripting::Screen::SetRenderHeight(int height)
{
	Screen::height = height;
}

void Engine::Scripting::Screen::SetRenderScale(Engine::Components::Vector2 scale)
{
	Screen::width = scale.x;
	Screen::height = scale.y;
}

Engine::Components::Vector2 Engine::Scripting::Screen::GetRenderScale()
{
	return Engine::Components::Vector2(width, height);
}

int Engine::Scripting::Screen::GetRenderWidth()
{
	return width;
}

int Engine::Scripting::Screen::GetRenderHeight()
{
	return height;
}

bool Engine::Scripting::Screen::IsVirtualResolutionInUse()
{
	return useVirtualResolution;
}
