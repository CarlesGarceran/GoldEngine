#include "Includes.h"
#include "GlIncludes.h"
#include "Time.h"
#include "SDK.h"
#include "Objects/Physics/CollisionType.h"
#include "Objects/Physics/Native/NativePhysicsService.h"
#include "Objects/Physics/RigidBody.h"
#include "Objects/Physics/PhysicsService.h"

using namespace Engine::Scripting;
using PhysicsService = Engine::EngineObjects::Physics::PhysicsService;

void Time::Wait(double sleepTime)
{
	return RAYLIB::WaitTime(sleepTime);
}

float Engine::Scripting::Time::deltaTime::get()
{
	return _timeScale * RAYLIB::GetFrameTime();
}

float Engine::Scripting::Time::unscaledDeltaTime::get()
{
	return RAYLIB::GetFrameTime();
}

double Engine::Scripting::Time::time::get()
{
	return _timeScale * RAYLIB::GetTime();
}

double Engine::Scripting::Time::unscaledTime::get()
{
	return RAYLIB::GetTime();
}

float Engine::Scripting::Time::timeScale::get()
{
	return _timeScale;
}

void Engine::Scripting::Time::timeScale::set(float value)
{
	_timeScale = value;
}

float Engine::Scripting::Time::fixedDeltaTime::get()
{
	float fixedTimeStep = 60.0f / 1000;

#ifdef USE_BULLET_PHYS
	PhysicsService^ physService = nullptr;
	if (Singleton<PhysicsService^>::Instantiated)
	{
		physService = Singleton<PhysicsService^>::Instance;
		fixedTimeStep = physService->frameRate / 1000;
	}
#endif

	return fixedTimeStep * _timeScale;
}

float Engine::Scripting::Time::unscaledFixedDeltaTime::get()
{
	float fixedTimeStep = 60.0f / 1000;

#ifdef USE_BULLET_PHYS
	PhysicsService^ physService = nullptr;
	if (Singleton<PhysicsService^>::Instantiated)
	{
		physService = Singleton<PhysicsService^>::Instance;
		fixedTimeStep = physService->frameRate / 1000;
	}
#endif

	return fixedTimeStep;
}
