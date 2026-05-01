#include "../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionType.h"
#include "Native/NativePhysicsService.h"
#include "RigidBody.h"
#include "PhysicsService.h"
#include "../../native/CollisionShape.h"
#include "Triggers/Collider.h"

using namespace System;
using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Native;

#pragma managed(push, off)

inline btCompoundShape* _createCompoundShape(btVector3 _bounds = btVector3(1, 1, 1))
{
	btBoxShape* shape = new btBoxShape(_bounds);
	btCompoundShape* compoundShape = new btCompoundShape();

	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin({ 0,0,0 });
	compoundShape->addChildShape(localTransform, shape);

	shape->setMargin(0.0f);

	return compoundShape;
}

inline void destroyCompoundShape(btCompoundShape* compound)
{
	for (int i = 0; i < compound->getNumChildShapes(); i++)
	{
		delete compound->getChildShape(i);
	}

	delete compound;
}

inline void disposeRigidBody(btRigidBody* rigidBody)
{
	if (!rigidBody) return;

	btMotionState* motionState = rigidBody->getMotionState();
	if (motionState)
		delete motionState;

	delete rigidBody;
}

inline std::array<float, 3> computeInertia(btCollisionShape* shape, float mass)
{
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);

	return { inertia.x() , inertia.y() , inertia.z() };
}

inline void setMass(btRigidBody* rigidBody, float mass, std::array<float, 3> inertia)
{
	btVector3 _inertia(inertia[0], inertia[1], inertia[2]);

	if (mass != 0.0f)
	{
		btCollisionShape* collisionShape = rigidBody->getCollisionShape();
		collisionShape->calculateLocalInertia(mass, _inertia);
	}

	rigidBody->setMassProps(mass, _inertia);
	rigidBody->updateInertiaTensor();
}

inline void _setAngularVelocity(btRigidBody* rigidBody, float x, float y, float z)
{
	rigidBody->setAngularVelocity({ x,y,z });

	rigidBody->activate(true);
}

inline void _setLinearVelocity(btRigidBody* rigidBody, float x, float y, float z)
{
	rigidBody->setLinearVelocity({ x,y,z });

	rigidBody->activate(true);
}

inline void SetRigidBodyTransform(btRigidBody* body, float position[3], float rotation[4])
{
	if (!body) return;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin({ position[0], position[1], position[2] });
	transform.setRotation({ rotation[0], rotation[1], rotation[2], rotation[3] });

	// Update both MotionState and body

	auto motionState = body->getMotionState();
	if (motionState == nullptr) return;

	motionState->setWorldTransform(transform);
	body->setWorldTransform(transform);

	body->activate();
}

inline void TranslateRigidBody(btRigidBody* body, float x, float y, float z)
{
	if (!body) return;

	btTransform transform;

	auto motionState = body->getMotionState();
	if (motionState == nullptr) return;

	motionState->getWorldTransform(transform);

	btVector3 offset(x, y, z);
	transform.setOrigin(transform.getOrigin() + offset);

	motionState->setWorldTransform(transform);
	body->setWorldTransform(transform);

	body->activate();
}

inline void RotateRigidBody(btRigidBody* body, float x, float y, float z, float w)
{
	if (!body) return;

	btTransform transform;
	auto motionState = body->getMotionState();

	if (motionState == nullptr) return; 
	motionState->getWorldTransform(transform);

	btQuaternion deltaRotation(x, y, z, w);
	btQuaternion current = transform.getRotation();
	current = deltaRotation * current;

	transform.setRotation(current);

	motionState->setWorldTransform(transform);
	body->setWorldTransform(transform);

	body->activate();
}

inline void SetRigidBodyPosition(btRigidBody* body, float x, float y, float z)
{
	if (!body) return;

	btMotionState* ms = body->getMotionState();
	if (!ms) return;

	btTransform transform;
	ms->getWorldTransform(transform);

	transform.setOrigin(btVector3(x, y, z));

	body->setWorldTransform(transform);
	ms->setWorldTransform(transform);
	body->setInterpolationWorldTransform(transform);

	body->activate(true);
}

inline void SetRigidBodyRotation(btRigidBody* body, float x, float y, float z, float w)
{
	if (!body) return;

	btMotionState* ms = body->getMotionState();
	if (!ms) return;

	btTransform transform;
	ms->getWorldTransform(transform);

	transform.setRotation(btQuaternion(x, y, z, w));

	body->setWorldTransform(transform);
	ms->setWorldTransform(transform);
	body->setInterpolationWorldTransform(transform);

	body->activate();
}

inline btRigidBody* createPhysBody(std::array<float, 3> originPosition, float mass, Engine::Native::CollisionShape* hookedShape, bool& dispose)
{
	btCollisionShape* collisionShape;
	dispose = false;

	if (hookedShape == nullptr || !hookedShape->hasCollisionShape())
	{
		hookedShape->createCollisionShape(_createCompoundShape(btVector3(1,1,1)));
		dispose = true;
	}

	collisionShape = hookedShape->getCollisionShape();

	btVector3 inertia(0, 0, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(originPosition[0], originPosition[1], originPosition[2]));

	bool isDynamic = (mass != 0);

	if (isDynamic)
		collisionShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, collisionShape, inertia);

	btRigidBody* rigidBody = new btRigidBody(constructionInfo);

	rigidBody->activate(true);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setWorldTransform(transform);

	return rigidBody;
}

inline std::array<float, 3> getPosition(btRigidBody* rigidBody)
{
	btTransform _transform;
	const btMotionState* ms = rigidBody->getMotionState();
	if (ms) ms->getWorldTransform(_transform);
	else _transform = rigidBody->getWorldTransform();

	return {
		_transform.getOrigin().getX(),
		_transform.getOrigin().getY(),
		_transform.getOrigin().getZ()
	};
}

inline std::array<float, 4> getRotation(btRigidBody* rigidBody)
{
	btTransform _transform;
	const btMotionState* ms = rigidBody->getMotionState();
	if (ms) ms->getWorldTransform(_transform);
	else _transform = rigidBody->getWorldTransform();

	btQuaternion q = _transform.getRotation();

	return { 
		q.x(),
		q.y(),
		q.z(),
		q.w() 
	};
}

inline std::array<float, 7> getVector(btRigidBody* rigidBody)
{
	btTransform _transform = rigidBody->getWorldTransform();

	std::array<float, 7> data = std::array<float, 7>();

	data[0] = _transform.getOrigin().getX();
	data[1] = _transform.getOrigin().getY();
	data[2] = _transform.getOrigin().getZ();

	btQuaternion q = _transform.getRotation();

	data[3] = q.x();
	data[4] = q.y();
	data[5] = q.z();
	data[6] = q.w();

	return data;
}

inline void _resetVelocity(btRigidBody* body)
{
	if (!body) return;

	body->setLinearVelocity(btVector3(0, 0, 0));
	body->setAngularVelocity(btVector3(0, 0, 0));
	body->clearForces();
}

inline void setVector(btRigidBody* rigidBody, float x, float y, float z)
{
	if (rigidBody == nullptr)
		return;

	btTransform transform;
	rigidBody->getMotionState()->getWorldTransform(transform);

	transform.setOrigin(btVector3(x, y, z));

	rigidBody->setWorldTransform(transform);
	rigidBody->getMotionState()->setWorldTransform(transform);
	rigidBody->setInterpolationWorldTransform(transform);

	rigidBody->activate(true);
}

inline void setVector(btRigidBody* rigidBody, float x, float y, float z, float qX, float qY, float qZ, float qW)
{
	if (rigidBody == nullptr)
		return;

	btTransform transform;
	rigidBody->getMotionState()->getWorldTransform(transform);

	btQuaternion identity = btQuaternion(qX, qY, qZ, qW);

	transform.setRotation(identity);
	transform.setOrigin(btVector3(x, y, z));

	rigidBody->setWorldTransform(transform);
	rigidBody->getMotionState()->setWorldTransform(transform);
	rigidBody->setInterpolationWorldTransform(transform);

	rigidBody->activate(true);
}

inline void _clearForces(btRigidBody*& rigidBody)
{
	rigidBody->setLinearVelocity(btVector3(0, 0, 0));
	rigidBody->clearForces();

	rigidBody->activate(true);
}

inline void _addForce(btRigidBody* rigidBody, float x, float y, float z, int forceMode)
{
	if (forceMode == 0)
	{
		rigidBody->applyCentralImpulse(btVector3(x, y, z));
	}
	else if (forceMode == 1)
	{
		rigidBody->applyCentralForce(btVector3(x, y, z));
	}

	rigidBody->activate(true);
}

inline void _addTorque(btRigidBody* rigidBody, float x, float y, float z, int forceMode)
{
	if (forceMode == 0)
	{
		rigidBody->applyTorqueImpulse(btVector3(x, y, z));
	}
	else if (forceMode == 1)
	{
		rigidBody->applyTorque(btVector3(x, y, z));
	}

	rigidBody->activate(true);
}

inline void _setAngularFactor(btRigidBody* rigidBody, float x, float y, float z)
{
	rigidBody->setAngularFactor({
		x,
		y,
		z
		});

	rigidBody->activate(true);
}

inline void _SyncFromPhysics(
	btRigidBody* body,
	float& px, float& py, float& pz,
	float& qx, float& qy, float& qz, float& qw)
{
	if (!body) return;
	const btMotionState* ms = body->getMotionState();
	btTransform t = {};

	if (ms)
	{
		ms->getWorldTransform(t);
	}
	else
	{
		t = body->getWorldTransform();
	}

	const btVector3& p = t.getOrigin();
	const btQuaternion& q = t.getRotation();

	px = p.getX();
	py = p.getY();
	pz = p.getZ();

	qx = q.getX();
	qy = q.getY();
	qz = q.getZ();
	qw = q.getW();
}

inline void _SyncToPhysics(
	btRigidBody* body,
	float px, float py, float pz,
	float qx, float qy, float qz, float qw)
{
	if (!body) return;

	btTransform t;
	t.setIdentity();

	t.setOrigin(btVector3(px, py, pz));
	t.setRotation(btQuaternion(qx, qy, qz, qw));

	btMotionState* ms = body->getMotionState();

	if (ms)
		ms->setWorldTransform(t);

	body->setWorldTransform(t);
	body->setInterpolationWorldTransform(t);

	body->activate(true);
}

btCompoundShape* _createCompoundShape(float x = 1, float y = 1, float z = 1)
{
	return _createCompoundShape(btVector3(x, y, z));
}

#pragma managed(pop)

void Engine::EngineObjects::Physics::RigidBody::createRigidBody()
{
	if (!Parent) return;

	Engine::Native::CollisionShape* collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();
	PhysicsService^ physicsService = Singleton<PhysicsService^>::Instance;
	Engine::EngineObjects::Physics::Collider^ collider = Parent->FindFirstChild<Engine::EngineObjects::Physics::Collider^>();

	btRigidBody* rigidBody;
	bool dispose = false;

	if (collider != nullptr)
	{
		collider->ClaimOwnership(this);
		rigidBody = createPhysBody(
			{
				Parent->transform->position.x,
				Parent->transform->position.y,
				Parent->transform->position.z
			},
			Mass,
			(Engine::Native::CollisionShape*)collider->getCollisionShape(),
			dispose
		);

	}
	else
	{
		rigidBody = createPhysBody(
			{
				Parent->transform->position.x,
				Parent->transform->position.y,
				Parent->transform->position.z
			},
			Mass,
			collisionShape,
			dispose
		);
	}

	disposeCollisionShape = dispose;

	if (!rigidBody)
	{
		printError("Failed to create btRigidbody (missing collision shape)");
		return;
	}

	collisionShape->setCollisionObject(rigidBody, false);

	if (Kinematic)
	{
		setMass(getRigidBody(), 0.0f, { 0,0,0 });
		getRigidBody()->setFlags(getRigidBody()->getFlags() | btRigidBody::CF_KINEMATIC_OBJECT);
	}
	else
	{
		setMass(getRigidBody(), Mass, { 0,0,0 });
		getRigidBody()->setFlags(getRigidBody()->getFlags() & ~btRigidBody::CF_KINEMATIC_OBJECT);
	}

	getRigidBody()->activate(true);
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);

	Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);

	registered = true;
}

void Engine::EngineObjects::Physics::RigidBody::PropertyChanged(String^ arg, Object^ _new, Object^ old)
{
	/*
	btRigidBody* rb = getRigidBody();

	if (rb == nullptr)
	{
		printError("btRigidbody* is not a reference to a valid pointer");
		return;
	}

	if (_new->Equals(old)) return;
	if (old->Equals(_new)) return;
	if (_new == old) return;

	// Keep inertia in case of a non-kinematic body
	if (Kinematic)
	{
		rb->clearForces();
		_setLinearVelocity(rb, 0, 0, 0);
		_setAngularVelocity(rb, 0, 0, 0);
	}

	if (arg == "position")
	{
		Engine::Components::Vector3 v3 = (Engine::Components::Vector3)_new;
		SetRigidBodyPosition(rb, v3.x, v3.y, v3.z);
	}
	else if (arg == "rotation")
	{
		Engine::Components::Quaternion v3 = (Engine::Components::Quaternion)_new;
		SetRigidBodyRotation(rb, v3.x, v3.y, v3.z, v3.w);
	}

	rb->activate(true);
	*/
	if (_new->Equals(old)) return;

	btRigidBody* rb = getRigidBody();

	if (rb == nullptr)
	{
		printError("btRigidbody* is not a reference to a valid pointer");
		return;
	}

	if (arg == "position" || arg == "rotation")
	{
		if (!EngineState::PlayMode)
		{
			if (Kinematic)
			{
				rb->clearForces();
				_setLinearVelocity(rb, 0, 0, 0);
				_setAngularVelocity(rb, 0, 0, 0);
			}

			if (arg == "position")
			{
				Engine::Components::Vector3 v3 = (Engine::Components::Vector3)_new;
				SetRigidBodyPosition(rb, v3.x, v3.y, v3.z);

				auto collider = FindFirstSibling<Collider^>();
				if (collider)
				{
					collider->transform->position = v3;
				}
			}
			else if (arg == "rotation")
			{
				Engine::Components::Quaternion v3 = (Engine::Components::Quaternion)_new;
				SetRigidBodyRotation(rb, v3.x, v3.y, v3.z, v3.w);

				auto collider = FindFirstSibling<Collider^>();
				if (collider)
				{
					collider->transform->rotation = v3;
				}
			}

			rb->activate(true);
		}

		transformDirty = true;
		teleport = false;
	}
}

void Engine::EngineObjects::Physics::RigidBody::reloadRigidbody()
{
	disposeRigidBody(getRigidBody());
	createRigidBody();
}

void Engine::EngineObjects::Physics::RigidBody::SyncFromPhysics()
{
	btRigidBody* body = getRigidBody();
	if (!body) return;

	float px, py, pz;
	float qx, qy, qz, qw;

	_SyncFromPhysics(body, px, py, pz, qx, qy, qz, qw);

	transform->position = Engine::Components::Vector3(px, py, pz);
	transform->rotation = Engine::Components::Quaternion(qx, qy, qz, qw);

	Parent->transform->position = transform->position;
	Parent->transform->rotation = transform->rotation;

	// Sync collider
	auto collider = FindFirstSibling<Collider^>();
	if (collider)
	{
		collider->transform->position = transform->position;
		collider->transform->rotation = transform->rotation;
	}
}

void Engine::EngineObjects::Physics::RigidBody::SyncToPhysics()
{
	btRigidBody* body = getRigidBody();
	if (!body) return;

	Engine::Components::Vector3 pos;
	Engine::Components::Quaternion rot;

	pos = transform->position;
	rot = transform->rotation;

	_SyncToPhysics(
		body,
		pos.x, pos.y, pos.z,
		rot.x, rot.y, rot.z, rot.w
	);
}

void Engine::EngineObjects::Physics::RigidBody::swapCollisionShape(btCollisionShape* shape)
{
	if (shape == nullptr) return;
	btRigidBody* rb = getRigidBody();

	if (rb == nullptr) return;

	if (disposeCollisionShape) 
	{
		delete rb->getCollisionShape();
		rb->setCollisionShape(nullptr);
		disposeCollisionShape = false;
	}

	rb->setCollisionShape(shape);
	SetKinematic(Kinematic);
}

void Engine::EngineObjects::Physics::RigidBody::checkSynchronization(bool& synchronize)
{
	synchronize = false;
	Engine::Internal::Components::Transform^ lfTfm = GetLastFrameTransform();

	if (!transform->position.Equals(lfTfm->position))
	{
		synchronize = true;
	}
}

void Engine::EngineObjects::Physics::RigidBody::DisposedShape()
{
	swapCollisionShape(
		_createCompoundShape(1, 1, 1)
	);

	disposeCollisionShape = true;
}

void Engine::EngineObjects::Physics::RigidBody::onMassChanged(float newValue, float oldValue)
{
	if (newValue == oldValue)
		return;

	setMass(getRigidBody(), newValue, { 0,0,0 });
}

void Engine::EngineObjects::Physics::RigidBody::RecalculateInertia()
{
	setMass(getRigidBody(), Mass, { 0,0,0 });
}

void Engine::EngineObjects::Physics::RigidBody::onKinematicChanged(bool newValue, bool oldValue)
{
	if (newValue == oldValue) return;

	SetKinematic(newValue);
}

void Engine::EngineObjects::Physics::RigidBody::Awake()
{
	// hook binding
	if (attributes->hasAttribute("Mass"))
		attributes->getAttribute("Mass")->onPropertyChanged->connect(gcnew System::Action<float, float>(this, &RigidBody::onMassChanged));

	if (attributes->hasAttribute("Kinematic"))
		attributes->getAttribute("Kinematic")->onPropertyChanged->connect(gcnew Action<bool, bool>(this, &RigidBody::onKinematicChanged));

	// hook property changed
	this->onPropertyChanged->connect(gcnew Action<String^, Object^, Object^>(this, &RigidBody::PropertyChanged));
}

void RigidBody::Start()
{
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr) return;

	createRigidBody();
	
	if (Parent == nullptr || !FindFirstSibling<Collider^>())
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}
	else
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)FindFirstSibling<Collider^>()->getCollisionShape();

		if (!_collisionShape->hasCollisionShape()) return;
		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		FindFirstSibling<Collider^>()->ClaimOwnership(this);
		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}

	registered = true;
}

void RigidBody::Update()
{
	/*
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr) return;
	
	if (!getRigidBody()->isActive()) return;

	if (isDirty)
	{
		Singleton<PhysicsService^>::Instance->RemovePhysicsObject(getRigidBody());

		isDirty = false;

		Singleton<PhysicsService^>::Instance->AddPhysicsObject(getRigidBody());
	}

	std::array<float, 7> data = getVector(this->getRigidBody());

	Parent->transform->position = Engine::Components::Vector3(data[0], data[1], data[2]);
	Parent->transform->rotation = Engine::Components::Quaternion(data[3], data[4], data[5], data[6]);

	transform->position = Parent->transform->position;
	transform->rotation = Parent->transform->rotation;

	if (FindFirstSibling<Collider^>())
	{
		auto collider = FindFirstSibling<Collider^>();

		collider->transform->position = transform->position;
		collider->transform->rotation = transform->rotation;
	}

	if (Parent == nullptr || !FindFirstSibling<Collider^>())
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}
	else
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)FindFirstSibling<Collider^>()->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		FindFirstSibling<Collider^>()->ClaimOwnership(this);
		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}

	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
		return;

	btRigidBody* body = getRigidBody();
	if (!body)
	{
		createRigidBody();
		return;
	}

	// KINEMATIC

	if (Kinematic)
	{
		SyncToPhysics();
		transformDirty = false;
		return;
	}

	// DYNAMIC

	/*
	if (transformDirty)
	{
		SyncToPhysics();

		if (teleport)
		{
			_resetVelocity(body);
		}

		transformDirty = false;
		teleport = false;
	}
	else
	{
		
	}

	if (body->isActive())
	{
		SyncFromPhysics();
	}

	auto collider = FindFirstSibling<Collider^>();

	Engine::Native::CollisionShape* shape = nullptr;

	if (collider)
	{
		collider->ClaimOwnership(this);
		shape = (Engine::Native::CollisionShape*)collider->getCollisionShape();
	}
	else
	{
		shape = (Engine::Native::CollisionShape*)this->getCollisionShape();
	}

	if (shape && shape->hasCollisionShape())
	{
		if (body->getCollisionShape() != shape->getCollisionShape())
		{
			swapCollisionShape(shape->getCollisionShape());
		}
	}

	*/

	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
	return;

	btRigidBody* body = getRigidBody();
	if (!body)
	{
		createRigidBody();
		return;
	}

	// Check if the transform needs synchronization
	bool needsSynchronization = false;
	checkSynchronization(needsSynchronization);

	if (needsSynchronization)
	{
		SyncToPhysics();
	}

	if (Kinematic)
	{
		if (!needsSynchronization) SyncToPhysics();
		transformDirty = false;
		return;
	}

	// Synchronize physics to render

	if (body->isActive())
	{
		SyncFromPhysics();
	}

	auto collider = FindFirstSibling<Collider^>();

	Engine::Native::CollisionShape* shape = nullptr;

	if (collider)
	{
		collider->ClaimOwnership(this);
		shape = (Engine::Native::CollisionShape*)collider->getCollisionShape();
	}
	else
	{
		shape = (Engine::Native::CollisionShape*)this->getCollisionShape();
	}

	if (shape && shape->hasCollisionShape())
	{
		if (body->getCollisionShape() != shape->getCollisionShape())
		{
			swapCollisionShape(shape->getCollisionShape());
		}
	}
}

void Engine::EngineObjects::Physics::RigidBody::PhysicsUpdate()
{
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
		return;

	btRigidBody* body = getRigidBody();
	if (!body)
	{
		createRigidBody();
		return;
	}

	// Synchronize physics to render

	if (body->isActive())
	{
		SyncFromPhysics();
	}
}

void Engine::EngineObjects::Physics::RigidBody::Draw()
{
	/*
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
		return;

	btRigidBody* body = getRigidBody();
	if (!body)
	{
		createRigidBody();
		return;
	}

	if (transformDirty)
	{
		SyncToPhysics();

		if (teleport)
		{
			_resetVelocity(body);
		}

		transformDirty = false;
		teleport = false;
	}
	*/
}

void Engine::EngineObjects::Physics::RigidBody::Destroy()
{
	btRigidBody* rb = getRigidBody();

	if (rb == nullptr) return;

	if (disposeCollisionShape)
	{
		auto shape = rb->getCollisionShape();

		if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
		{
			destroyCompoundShape((btCompoundShape*)shape);
		}
		else
		{
			delete shape;
		}

		rb->setCollisionShape(nullptr);
	}

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->Disown();
	}
}

void RigidBody::OnInactive()
{
	if (getRigidBody() == nullptr)
		return;
	btRigidBody* rb = getRigidBody();

	if (getRigidBody()->isActive())
	{
		setVector(rb, (float)transform->position.x, (float)transform->position.y, (float)transform->position.z, (float)transform->rotation.x, (float)transform->rotation.y, (float)transform->rotation.z, (float)transform->rotation.w);
		getRigidBody()->setActivationState(DISABLE_SIMULATION);
		Singleton<PhysicsService^>::Instance->RemovePhysicsObject(this);
	}

	setVector(rb, (float)transform->position.x, (float)transform->position.y, (float)transform->position.z, (float)transform->rotation.x, (float)transform->rotation.y, (float)transform->rotation.z, (float)transform->rotation.w);
}

void RigidBody::AddForce(Engine::Components::Vector3 position, Enums::ForceMode mode)
{
	if (getRigidBody() == nullptr)
		return;

	_addForce(getRigidBody(), position.x, position.y, position.z, (int)mode);
}

void Engine::EngineObjects::Physics::RigidBody::AddTorque(Engine::Components::Vector3 direction, Enums::ForceMode mode)
{
	if (getRigidBody() == nullptr)
		return;

	_addTorque(getRigidBody(), direction.x, direction.y, direction.z, (int)mode);
}

void RigidBody::OnActive()
{
	if (getRigidBody() == nullptr)
		return;

	if (!getRigidBody()->isActive())
	{
		getRigidBody()->activate(true);
		getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
		Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);
	}
}

void Engine::EngineObjects::Physics::RigidBody::clearForces()
{
	btRigidBody* rigidBody = getRigidBody();

	_clearForces(rigidBody);

	Engine::EngineObjects::Physics::Native::updateCollisionObject(
		rigidBody,
		{
			Parent->transform->position.x,
			Parent->transform->position.y,
			Parent->transform->position.z
		},
		{
			Parent->transform->rotation.x,
			Parent->transform->rotation.y,
			Parent->transform->rotation.z
		},
		{
			transform->scale.x,
			transform->scale.y,
			transform->scale.z
		}
	);
}

void Engine::EngineObjects::Physics::RigidBody::setAngularVelocity(Engine::Components::Vector3 velocity)
{
	if (getRigidBody() == nullptr)
		return;

	_setAngularVelocity(getRigidBody(), velocity.x, velocity.y, velocity.z);
}

void Engine::EngineObjects::Physics::RigidBody::setLinearVelocity(Engine::Components::Vector3 velocity)
{
	if (getRigidBody() == nullptr)
		return;

	_setLinearVelocity(getRigidBody(), velocity.x, velocity.y, velocity.z);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getCenterOfMass()
{
	return Engine::Components::Vector3(
		getRigidBody()->getCenterOfMassPosition().x(),
		getRigidBody()->getCenterOfMassPosition().y(),
		getRigidBody()->getCenterOfMassPosition().z()
	);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getTotalForce()
{
	return Engine::Components::Vector3(
		getRigidBody()->getTotalForce().x(),
		getRigidBody()->getTotalForce().y(),
		getRigidBody()->getTotalForce().z()
	);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getTotalTorque()
{
	return Engine::Components::Vector3(
		getRigidBody()->getTotalTorque().x(),
		getRigidBody()->getTotalTorque().y(),
		getRigidBody()->getTotalTorque().z()
	);
}

void Engine::EngineObjects::Physics::RigidBody::SetTransform(Engine::Components::Vector3 position, Engine::Components::Quaternion rotation)
{
	btRigidBody* body = getRigidBody();

	if (!body) return;

	float pos[3] = { position.x, position.y, position.z };
	float rot[4] = { rotation.x, rotation.y, rotation.z, rotation.w };

	SetRigidBodyTransform(body, pos, rot);
}

void Engine::EngineObjects::Physics::RigidBody::Translate(Engine::Components::Vector3 position)
{
	btRigidBody* body = getRigidBody();

	if (!body) return;

	TranslateRigidBody(body, position.x, position.y, position.z);
}

void Engine::EngineObjects::Physics::RigidBody::Rotate(Engine::Components::Quaternion rotation)
{
	btRigidBody* body = getRigidBody();

	if (!body) return;

	RotateRigidBody(body, rotation.x, rotation.y, rotation.z, rotation.w);
}

void Engine::EngineObjects::Physics::RigidBody::SetPosition(Engine::Components::Vector3 position)
{
	transformDirty = true;
	teleport = true;

	transform->position = position;
	SyncToPhysics();
}

void Engine::EngineObjects::Physics::RigidBody::SetRotation(Engine::Components::Quaternion rotation)
{
	transformDirty = true;
	teleport = true;

	transform->rotation = rotation;
	SyncToPhysics();
}

void Engine::EngineObjects::Physics::RigidBody::SetKinematic(bool isKinematic)
{
	Kinematic = isKinematic;
	btRigidBody* rigidBody = getRigidBody();
	if (rigidBody == nullptr) return;

	int flags = rigidBody->getCollisionFlags();

	if (isKinematic)
	{
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
		rigidBody->setCollisionFlags(flags | btCollisionObject::CF_KINEMATIC_OBJECT);

		setMass(rigidBody, 0.0f, { 0,0,0 });
		_setAngularVelocity(rigidBody, 0, 0, 0);
		_setLinearVelocity(rigidBody, 0, 0, 0);
		rigidBody->clearForces();
	}
	else
	{
		rigidBody->setCollisionFlags(flags & ~btCollisionObject::CF_KINEMATIC_OBJECT);

		rigidBody->forceActivationState(ACTIVE_TAG);
		rigidBody->activate();

		btCollisionShape* shape = rigidBody->getCollisionShape();

		setMass(rigidBody, Mass, {0, 0, 0});
		rigidBody->updateInertiaTensor();

		_setAngularVelocity(rigidBody, 0, 0, 0);
		_setLinearVelocity(rigidBody, 0, 0, 0);
		rigidBody->clearForces();
	}
}

btRigidBody* RigidBody::getRigidBody()
{
	auto wrapper = this->getCollisionShape();
	if (!wrapper || !wrapper->hasCollisionObject())
		return nullptr;

	return static_cast<btRigidBody*>(wrapper->getCollisionObject());
}

void Engine::EngineObjects::Physics::RigidBody::SetBounciness(float bounciness)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setRestitution(bounciness);
}

float Engine::EngineObjects::Physics::RigidBody::GetBounciness()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getRestitution();
	else
		return 0.0f;
}

void Engine::EngineObjects::Physics::RigidBody::SetFriction(float friction)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setFriction(friction);
}

void Engine::EngineObjects::Physics::RigidBody::SetLinearDamping(float value)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(value, getRigidBody()->getAngularDamping());
}

void Engine::EngineObjects::Physics::RigidBody::SetAngularDamping(float value)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(getRigidBody()->getLinearDamping(), value);
}

void Engine::EngineObjects::Physics::RigidBody::SetDamping(float linearDamping, float angularDamping)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(linearDamping, angularDamping);
}

void Engine::EngineObjects::Physics::RigidBody::SetAngularFactor(Engine::Components::Vector3 axis)
{
	if (getRigidBody() != nullptr)
		_setAngularFactor(getRigidBody(), axis.x, axis.y, axis.z);
}

float Engine::EngineObjects::Physics::RigidBody::GetLinearDamping()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getLinearDamping();
	else
		return 0.0f;
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::GetLinearVelocity()
{
	if (getRigidBody() != nullptr)
		return Engine::Components::Vector3(getRigidBody()->getLinearVelocity().getX(), getRigidBody()->getLinearVelocity().getY(), getRigidBody()->getLinearVelocity().getZ());
	else
		return Engine::Components::Vector3::Zero();
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::GetAngularFactor()
{
	if (getRigidBody() != nullptr)
		return Engine::Components::Vector3(getRigidBody()->getAngularFactor().getX(), getRigidBody()->getAngularFactor().getY(), getRigidBody()->getAngularFactor().getZ());
	else
		return Engine::Components::Vector3::Zero();
}

float Engine::EngineObjects::Physics::RigidBody::GetAngularDamping()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getAngularDamping();
	else
		return 0.0f;
}

float Engine::EngineObjects::Physics::RigidBody::GetFriction()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getFriction();
	else
		return 0.0f;
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionEnter(GameObject^ instance)
{
	Parent->OnCollisionEnter(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionEnter(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionStay(GameObject^ instance)
{
	Parent->OnCollisionStay(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionStay(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionExit(GameObject^ instance)
{
	Parent->OnCollisionExit(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionExit(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerEnter(GameObject^ instance)
{
	Parent->OnTriggerEnter(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionEnter(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerStay(GameObject^ instance)
{
	Parent->OnTriggerStay(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionStay(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerExit(GameObject^ instance)
{
	Parent->OnTriggerExit(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionExit(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollided(GameObject^ instance)
{
	Parent->OnCollided(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollided(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggered(GameObject^ instance)
{
	Parent->OnTriggered(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollided(instance);
	}
}

#endif
