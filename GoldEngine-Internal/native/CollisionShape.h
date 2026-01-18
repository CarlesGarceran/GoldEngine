#pragma once

#ifdef USE_BULLET_PHYS

#pragma managed(push, off)
#include <btBulletCollisionCommon.h>
#pragma managed(pop)

namespace Engine::Native
{
	class CollisionShape
	{
	private:
		Engine::Native::EnginePtr<btCollisionShape*>* collisionShape;
		Engine::Native::EnginePtr <btCollisionObject*>* collisionObject;
		System::Runtime::InteropServices::GCHandle handle;
		void* userHandler;

	public:
		CollisionShape(Engine::Internal::Components::GameObject^);
		~CollisionShape();

		void createCollisionShape(btCollisionShape* shape);
		void createBulletObject(bool uploadToService = true);
		void createBulletGhostObject(bool uploadToService = true);

		void setCollisionObject(btRigidBody* collisionObject, bool uploadToService = true);
		void setCollisionObject(btCollisionObject* collisionObject, bool uploadToService = true, bool bindDeleter = true);

		btCollisionObject*& getCollisionObject();
		btCollisionShape*& getCollisionShape();

		bool hasCollisionObject();
		bool hasCollisionShape();

		void resampleAABB();
		void freeCollisionObject();

		void setGameObject(GameObject^ instance);
	};
}

#endif