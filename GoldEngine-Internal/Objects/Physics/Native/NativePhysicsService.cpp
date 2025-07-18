#include "../../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "NativePhysicsService.h"

using namespace Engine::EngineObjects::Physics::Native;


// unmanaged code... 
// bs in general...
#pragma managed(push, off)

typedef enum NativeCollisionType
{
	Concave,
	Convex,
	BoundingBox
};

btCollisionShape* createCollisionBox(float x, float y, float z)
{
	return new btBoxShape(btVector3(x, y, z));
}

btCollisionShape* createCollisionSphere(float radius)
{
	return new btSphereShape(radius);
}

btCollisionShape* createCollisionMesh(RAYLIB::Mesh mesh, int collisionType)
{
	btCollisionShape* collisionShape = nullptr;

	if (collisionType == (int)NativeCollisionType::Convex)
	{
		collisionShape = new btConvexHullShape();

		for (int i = 0; i < mesh.vertexCount; i++)
		{
			float x = mesh.vertices[i * 3];
			float y = mesh.vertices[i * 3 + 1];
			float z = mesh.vertices[i * 3 + 2];

			((btConvexHullShape*)collisionShape)->addPoint(btVector3(x, y, z), false);
		}

		((btConvexHullShape*)collisionShape)->recalcLocalAabb();
	}
	else if (collisionType == (int)NativeCollisionType::Concave)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (int i = 0; i < mesh.triangleCount; i++)
		{
			int index0 = mesh.indices[i * 3];
			int index1 = mesh.indices[i * 3 + 1];
			int index2 = mesh.indices[i * 3 + 2];

			btVector3 v0(
				mesh.vertices[index0 * 3],
				mesh.vertices[index0 * 3 + 1],
				mesh.vertices[index0 * 3 + 2]);

			btVector3 v1(
				mesh.vertices[index1 * 3],
				mesh.vertices[index1 * 3 + 1],
				mesh.vertices[index1 * 3 + 2]);

			btVector3 v2(
				mesh.vertices[index2 * 3],
				mesh.vertices[index2 * 3 + 1],
				mesh.vertices[index2 * 3 + 2]);

			triangleMesh->addTriangle(v0, v1, v2);
		}

		collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);
	}
	else if (collisionType == (int)NativeCollisionType::BoundingBox)
	{
		RAYLIB::BoundingBox boundingBox = RAYLIB::GetMeshBoundingBox(mesh);

		collisionShape = createCollisionBox(boundingBox.max.x, boundingBox.max.y, boundingBox.max.z);
	}

	return collisionShape;
}

#pragma managed(pop)


btCollisionShape* NativePhysicsService::getCollisionShapeFromID(unsigned int modelId, unsigned int meshId, int collisionType)
{
	RAYLIB::Mesh mesh = (Mesh)DataPacks::singleton().GetModel(modelId).meshes[meshId];

	btCollisionShape* collisionShape = createCollisionMesh(mesh, (int)collisionType);

	return collisionShape;
}

btCollisionShape* NativePhysicsService::getCollisionShapeFromMesh(RAYLIB::Mesh mesh, int collisionType)
{
	btCollisionShape* collisionShape = createCollisionMesh(mesh, (int)collisionType);
	return collisionShape;
}


btCollisionShape* NativePhysicsService::getCollisionShapeForBox(float x, float y, float z)
{
	return createCollisionBox(x, y, z);
}

btCollisionShape* NativePhysicsService::getCollisionShapeForSphere(float radius)
{
	return createCollisionSphere(radius);
}


NativePhysicsService::NativePhysicsService()
{
	NativeSingleton<NativePhysicsService*>::create(this);
}

#endif