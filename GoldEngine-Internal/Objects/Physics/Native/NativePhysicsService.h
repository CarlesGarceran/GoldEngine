#pragma once

#ifdef USE_BULLET_PHYS
UNMANAGED_BEGIN

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#pragma managed(pop)

namespace Engine::EngineObjects::Physics::Native
{
	void getOpenGLMatrix(btCollisionObject* collisionObject, float outMatrix[16]);
	void updateCollisionObject(btCollisionObject* collisionObject, std::array<float, 3> position, std::array<float, 3> euler);
	void updateCollisionObject(btCollisionObject* collisionObject, std::array<float, 3> position, std::array<float, 3> euler, std::array<float, 3> scale);
	void updateCollisionObject(btRigidBody* collisionObject, std::array<float, 3> position, std::array<float, 3> euler, std::array<float, 3> scale);

	private class NativePhysicsService
	{
	private:
		std::vector<btCompoundShape*> compoundShapes;

	public:
		NativePhysicsService();
		~NativePhysicsService();

		btCollisionShape* getCollisionShapeFromID(unsigned int modelId, unsigned int meshId, int collisionType);
		btCollisionShape* getCollisionShapeFromMesh(RAYLIB::Mesh mesh, int collisionType);

		btCollisionShape* getCollisionShapeForBox(float x, float y, float z);
		btCollisionShape* getCollisionShapeForSphere(float radius);

		btCompoundShape* addCompoundShape(btCollisionShape* shape);
		btCompoundShape* getCompoundShape(int index);
		btCompoundShape* getCompoundShape(btCollisionShape* shape);

		void ShapeToMesh(btCollisionShape* shape, RAYLIB::Mesh& outputMesh, bool uploadToGPU = false);
		void ShapeToMesh(btCompoundShape* shape, RAYLIB::Mesh& outputMesh, bool uploadToGPU = false);
	};
}

UNMANAGED_END
#endif