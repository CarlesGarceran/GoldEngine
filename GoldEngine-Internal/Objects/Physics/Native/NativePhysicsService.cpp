#include "../../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "NativePhysicsService.h"

using namespace Engine::EngineObjects::Physics::Native;


// unmanaged code... 
// bs in general...
UNMANAGED_BEGIN

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btPolyhedralConvexShape.h>
#include <BulletCollision/CollisionShapes/btConvexPolyhedron.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>

inline void AllocateMeshData(Mesh* mesh, int triangleCount)
{
    mesh->vertexCount = triangleCount * 3;
    mesh->triangleCount = triangleCount;

    mesh->vertices = (float*)MemAlloc(mesh->vertexCount * 3 * sizeof(float));
    mesh->texcoords = (float*)MemAlloc(mesh->vertexCount * 2 * sizeof(float));
    mesh->normals = (float*)MemAlloc(mesh->vertexCount * 3 * sizeof(float));
}

typedef enum NativeCollisionType
{
	Concave,
	Convex,
	BoundingBox
};

btCollisionShape* createCollisionBox(float x, float y, float z)
{
	return new btBoxShape({ x,y,z });
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
        auto* shape = new btConvexHullShape();

        for (int i = 0; i < mesh.vertexCount; i++)
        {
            float x = mesh.vertices[i * 3];
            float y = mesh.vertices[i * 3 + 1];
            float z = mesh.vertices[i * 3 + 2];
            shape->addPoint(btVector3(x, y, z), false);
        }

        shape->optimizeConvexHull();
        shape->recalcLocalAabb();
        collisionShape = shape;
    }
    else if (collisionType == (int)NativeCollisionType::Concave)
    {
        auto* triangleMesh = new btTriangleMesh();

        if (mesh.indices != nullptr)
        {
            for (int i = 0; i < mesh.triangleCount; i++)
            {
                unsigned int idx0 = mesh.indices[i * 3 + 0];
                unsigned int idx1 = mesh.indices[i * 3 + 1];
                unsigned int idx2 = mesh.indices[i * 3 + 2];

                btVector3 v0(mesh.vertices[idx0 * 3 + 0], mesh.vertices[idx0 * 3 + 1], mesh.vertices[idx0 * 3 + 2]);
                btVector3 v1(mesh.vertices[idx1 * 3 + 0], mesh.vertices[idx1 * 3 + 1], mesh.vertices[idx1 * 3 + 2]);
                btVector3 v2(mesh.vertices[idx2 * 3 + 0], mesh.vertices[idx2 * 3 + 1], mesh.vertices[idx2 * 3 + 2]);

                triangleMesh->addTriangle(v0, v1, v2);
            }
        }
        else // Non-indexed mesh
        {
            int triangleCount = mesh.vertexCount / 3;
            for (int i = 0; i < triangleCount; i++)
            {
                btVector3 v0(mesh.vertices[i * 3 * 3 + 0], mesh.vertices[i * 3 * 3 + 1], mesh.vertices[i * 3 * 3 + 2]);
                btVector3 v1(mesh.vertices[i * 3 * 3 + 3], mesh.vertices[i * 3 * 3 + 4], mesh.vertices[i * 3 * 3 + 5]);
                btVector3 v2(mesh.vertices[i * 3 * 3 + 6], mesh.vertices[i * 3 * 3 + 7], mesh.vertices[i * 3 * 3 + 8]);

                triangleMesh->addTriangle(v0, v1, v2);
            }
        }

        auto* shape = new btGImpactMeshShape(triangleMesh);
        shape->setLocalScaling({ 1, 1, 1 });
        shape->updateBound();
        collisionShape = shape;
    }
    else if (collisionType == (int)NativeCollisionType::BoundingBox)
    {
        RAYLIB::BoundingBox boundingBox = RAYLIB::GetMeshBoundingBox(mesh);

        btVector3 halfExtents(
            (boundingBox.max.x - boundingBox.min.x) * 0.5f,
            (boundingBox.max.y - boundingBox.min.y) * 0.5f,
            (boundingBox.max.z - boundingBox.min.z) * 0.5f
        );

        collisionShape = new btBoxShape(halfExtents);
    }

    return collisionShape;
}


UNMANAGED_END

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

btCompoundShape* Engine::EngineObjects::Physics::Native::NativePhysicsService::addCompoundShape(btCollisionShape* shape)
{
    btCompoundShape* compoundShape = new btCompoundShape();

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin({ 0,0,0 });
    
    compoundShape->addChildShape(transform, shape);

    int shapeIndex = this->compoundShapes.size();
    this->compoundShapes[shapeIndex] = compoundShape;

    return compoundShape;
}

btCompoundShape* Engine::EngineObjects::Physics::Native::NativePhysicsService::getCompoundShape(int index)
{
    try
    {
        return this->compoundShapes.at(index);
    }
    catch (std::exception ex)
    {
        btCompoundShape* compoundShape = new btCompoundShape();
        compoundShapes[index] = compoundShape;

        return compoundShape;
    }
}

btCompoundShape* Engine::EngineObjects::Physics::Native::NativePhysicsService::getCompoundShape(btCollisionShape* shape)
{
    for (btCompoundShape* compoundShape : compoundShapes)
    {
        int numShapes = compoundShape->getNumChildShapes();
        for (int x = 0; x < numShapes; x++)
        {
            if (compoundShape->getChildShape(x) == shape)
                return compoundShape;
        }
    }

    btCompoundShape* compoundShape = new btCompoundShape();
    compoundShapes.push_back(compoundShape);
    return compoundShape;
}



void Engine::EngineObjects::Physics::Native::NativePhysicsService::ShapeToMesh(btCollisionShape* shape, RAYLIB::Mesh& mesh, bool uploadToGPU)
{
    if (shape->isConvex()) 
    {
        const btConvexPolyhedron* poly = 0;
        
        if (shape->isPolyhedral())
        {
            poly = ((btPolyhedralConvexShape*)shape)->getConvexPolyhedron();
        }
    
        if (poly)
        {
            int i;
            AllocateMeshData(&mesh, poly->m_faces.size());
            int currentVertice = 0;
            for (i = 0; i < poly->m_faces.size(); i++) {
                btVector3 centroid(0, 0, 0);
                int numVerts = poly->m_faces[i].m_indices.size();
                if (numVerts > 2) {
                    btVector3 v1 = poly->m_vertices[poly->m_faces[i].m_indices[0]];
                    for (int v = 0; v < poly->m_faces[i].m_indices.size() - 2; v++) {
                        btVector3 v2 = poly->m_vertices[poly->m_faces[i].m_indices[v + 1]];
                        btVector3 v3 = poly->m_vertices[poly->m_faces[i].m_indices[v + 2]];
                        btVector3 normal = (v3 - v1).cross(v2 - v1);
                        normal.normalize();

                        mesh.vertices[currentVertice] = v1.x();
                        mesh.vertices[currentVertice + 1] = v1.y();
                        mesh.vertices[currentVertice + 2] = v1.z();
                        mesh.normals[currentVertice] = normal.getX();
                        mesh.normals[currentVertice + 1] = normal.getY();
                        mesh.normals[currentVertice + 2] = normal.getZ();

                        mesh.vertices[currentVertice + 3] = v2.x();
                        mesh.vertices[currentVertice + 4] = v2.y();
                        mesh.vertices[currentVertice + 5] = v2.z();
                        mesh.normals[currentVertice + 3] = normal.getX();
                        mesh.normals[currentVertice + 4] = normal.getY();
                        mesh.normals[currentVertice + 5] = normal.getZ();

                        mesh.vertices[currentVertice + 6] = v3.x();
                        mesh.vertices[currentVertice + 7] = v3.y();
                        mesh.vertices[currentVertice + 8] = v3.z();
                        mesh.normals[currentVertice + 6] = normal.getX();
                        mesh.normals[currentVertice + 7] = normal.getY();
                        mesh.normals[currentVertice + 8] = normal.getZ();

                        currentVertice += 9;
                    }
                }
            }
        }
        else
        {
            btConvexShape* convexShape = (btConvexShape*)shape;
            btShapeHull* hull = new btShapeHull(convexShape);
            hull->buildHull(shape->getMargin());

            AllocateMeshData(&mesh, hull->numTriangles());
            int currentVertice = 0;
            if (hull->numTriangles() > 0) {

                int index = 0;
                const unsigned int* idx = hull->getIndexPointer();
                const btVector3* vtx = hull->getVertexPointer();

                for (int i = 0; i < hull->numTriangles(); i++) {
                    int i1 = index++;
                    int i2 = index++;
                    int i3 = index++;
                    btAssert(i1 < hull->numIndices() && i2 < hull->numIndices() &&
                        i3 < hull->numIndices());

                    int index1 = idx[i1];
                    int index2 = idx[i2];
                    int index3 = idx[i3];
                    btAssert(index1 < hull->numVertices() &&
                        index2 < hull->numVertices() &&
                        index3 < hull->numVertices());

                    btVector3 v1 = vtx[index1];
                    btVector3 v2 = vtx[index2];
                    btVector3 v3 = vtx[index3];
                    btVector3 normal = (v3 - v1).cross(v2 - v1);
                    normal.normalize();

                    mesh.vertices[currentVertice] = v1.x();
                    mesh.vertices[currentVertice + 1] = v1.y();
                    mesh.vertices[currentVertice + 2] = v1.z();
                    mesh.normals[currentVertice] = normal.getX();
                    mesh.normals[currentVertice + 1] = normal.getY();
                    mesh.normals[currentVertice + 2] = normal.getZ();

                    mesh.vertices[currentVertice + 3] = v2.x();
                    mesh.vertices[currentVertice + 4] = v2.y();
                    mesh.vertices[currentVertice + 5] = v2.z();
                    mesh.normals[currentVertice + 3] = normal.getX();
                    mesh.normals[currentVertice + 4] = normal.getY();
                    mesh.normals[currentVertice + 5] = normal.getZ();

                    mesh.vertices[currentVertice + 6] = v3.x();
                    mesh.vertices[currentVertice + 7] = v3.y();
                    mesh.vertices[currentVertice + 8] = v3.z();
                    mesh.normals[currentVertice + 6] = normal.getX();
                    mesh.normals[currentVertice + 7] = normal.getY();
                    mesh.normals[currentVertice + 8] = normal.getZ();

                    currentVertice += 9;
                }
            }
        }
    }

    if (uploadToGPU)
        UploadMesh(&mesh, false);
}

void Engine::EngineObjects::Physics::Native::NativePhysicsService::ShapeToMesh(btCompoundShape* shape, RAYLIB::Mesh& outputMesh, bool uploadToGPU)
{
    btCollisionShape* collisionShape = shape->getChildShape(0);
    ShapeToMesh(collisionShape, outputMesh, uploadToGPU);
}

NativePhysicsService::NativePhysicsService()
{
	NativeSingleton<NativePhysicsService*>::create(this);
}

Engine::EngineObjects::Physics::Native::NativePhysicsService::~NativePhysicsService()
{
	NativeSingleton<NativePhysicsService*>::free();
}

#endif

void Engine::EngineObjects::Physics::Native::getOpenGLMatrix(btCollisionObject* collisionObject, float outMatrix[16])
{
    collisionObject->getWorldTransform().getOpenGLMatrix(outMatrix);
}

void Engine::EngineObjects::Physics::Native::updateCollisionObject(btCollisionObject* collisionObject, std::array<float, 3> position, std::array<float, 3> euler)
{
	btTransform& _transform = collisionObject->getWorldTransform();

	btVector3 pos;
	pos.setValue(
		position[0],
		position[1],
		position[2]
	);

	btQuaternion quat;
	quat.setEuler(
		euler[1],
		euler[0],
		euler[2]
	);

	_transform.setOrigin(pos);
	_transform.setRotation(quat);
}

void Engine::EngineObjects::Physics::Native::updateCollisionObject(btCollisionObject* collisionObject, std::array<float, 3> position, std::array<float, 3> euler, std::array<float, 3> scale)
{
    btTransform& _transform = collisionObject->getWorldTransform();

    btVector3 pos;
    pos.setValue(
        position[0],
        position[1],
        position[2]
    );

    btQuaternion quat;
    quat.setEuler(
        euler[1],
        euler[0],
        euler[2]
    );

    btVector3 sca;
    sca.setValue(
        scale[0],
        scale[1],
        scale[2]
    );

    _transform.setOrigin(pos);
    _transform.setRotation(quat);
    collisionObject->getCollisionShape()->setLocalScaling(sca);
}

void Engine::EngineObjects::Physics::Native::updateCollisionObject(btRigidBody* collisionObject, std::array<float, 3> position, std::array<float, 3> euler, std::array<float, 3> scale)
{
    btTransform _transform;
    collisionObject->getMotionState()->getWorldTransform(_transform);

    btVector3 pos;
    pos.setValue(
        position[0],
        position[1],
        position[2]
    );

    btQuaternion quat;
    quat.setEuler(
        euler[1],
        euler[0],
        euler[2]
    );

    btVector3 sca;
    sca.setValue(
        scale[0],
        scale[1],
        scale[2]
    );

    _transform.setOrigin(pos);
    _transform.setRotation(quat);
    collisionObject->getCollisionShape()->setLocalScaling(sca);

    collisionObject->getMotionState()->setWorldTransform(_transform);
}