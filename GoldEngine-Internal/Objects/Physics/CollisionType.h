#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics::Enums
{
	public enum class CollisionType
	{
		Concave,
		Convex,
		BoundingBox
	};

	public enum class ColliderShape
	{
		Box,
		Sphere,
		Mesh
	};

	public enum class ColliderType
	{
		Collider,
		Trigger
	};

	public enum class ForceMode
	{
		Impulse,
		Force
	};
}

#endif