#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics::Enums
{
	public enum class ColliderShape
	{
		Box,
		Sphere,
		Mesh,
		Capsule,
		Cylinder,
		Cone
	};

	public enum class MeshCollisionType
	{
		Concave,
		Convex
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