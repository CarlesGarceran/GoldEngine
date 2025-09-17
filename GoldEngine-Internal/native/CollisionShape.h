#pragma once

#ifdef USE_BULLET_PHYS

#pragma managed(push, off)
#include <btBulletCollisionCommon.h>

namespace Engine::Native
{
	class CollisionShape
	{
	private:
		Engine::Native::EnginePtr<btCollisionShape*>* collisionShape;
		Engine::Native::EnginePtr <btCollisionObject*>* collisionObject;
		System::Runtime::InteropServices::GCHandle handle;
		void* userHandler;
		msclr::gcroot<GameObject^> gameObject;
		bool uploaded;

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

		GameObject^ getGameObject();
		void setGameObject(GameObject^ instance);
	};
}

#pragma managed(pop)

#endif