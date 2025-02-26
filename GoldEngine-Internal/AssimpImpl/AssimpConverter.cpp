#include <string>
#include <filesystem>
#include "../Macros.h"
#include "../Includes.h"
#include "../GlIncludes.h"
#include "../CastToNative.h"
#include "../LoggingAPI.h"
#include <assimp/BaseImporter.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/importerdesc.h>
#include <assimp/Exporter.hpp>
#include <assimp/cexport.h>
#include "AssimpConverter.h"

#pragma managed(push, off)

AssimpConverter::AssimpConverter(std::string fileName, std::string outputFile, std::string format)
{
    this->FileName = fileName;
    this->scene = nullptr;

    this->CreateMesh(aiProcess_Triangulate, outputFile, format);
}

AssimpConverter::AssimpConverter(std::string fileName, unsigned int flags, std::string outputFile, std::string format)
{
    this->FileName = fileName;
    this->scene = nullptr;

    this->CreateMesh(flags, outputFile, format);
}

void SetMeshData(RAYLIB::Mesh& rlMesh, aiMesh* ai_mesh)
{
    rlMesh.vertexCount = ai_mesh->mNumVertices;
    rlMesh.vertices = (float*)malloc(rlMesh.vertexCount * 3 * sizeof(float));

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
    {
        rlMesh.vertices[i * 3 + 0] = ai_mesh->mVertices[i].x;
        rlMesh.vertices[i * 3 + 1] = ai_mesh->mVertices[i].y;
        rlMesh.vertices[i * 3 + 2] = ai_mesh->mVertices[i].z;
    }

    if (ai_mesh->mTextureCoords[1])
    {
        rlMesh.texcoords2 = (float*)malloc((sizeof(float) * rlMesh.vertexCount) * 2);
        unsigned int texCoord = 0;
        for (int j = 0; j < rlMesh.vertexCount * 2; j += 2)
        {
            rlMesh.texcoords2[j] = ai_mesh->mTextureCoords[1][texCoord].x;
            rlMesh.texcoords2[j + 1] = ai_mesh->mTextureCoords[1][texCoord].y;
            texCoord++;
        }
    }

    rlMesh.normals = (float*)malloc((sizeof(float) * rlMesh.vertexCount) * 3);
    unsigned int normalCounter = 0;
    for (int j = 0; j < rlMesh.vertexCount * 3; j += 3)
    {
        rlMesh.normals[j] = ai_mesh->mNormals[normalCounter].x;
        rlMesh.normals[j + 1] = ai_mesh->mNormals[normalCounter].y;
        rlMesh.normals[j + 2] = ai_mesh->mNormals[normalCounter].z;
        normalCounter++;
    }

    unsigned int indiceTotal = 0;
    for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++)
    {
        indiceTotal += ai_mesh->mFaces[j].mNumIndices;
    }

    rlMesh.indices = (unsigned short*)malloc(sizeof(unsigned short) * indiceTotal);
    unsigned int indexCounter = 0;
    for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++)
    {
        for (unsigned int k = 0; k < ai_mesh->mFaces[j].mNumIndices; k++)
        {
            rlMesh.indices[indexCounter] = ai_mesh->mFaces[j].mIndices[k];
            indexCounter++;
        }
    }

    rlMesh.triangleCount = ai_mesh->mNumFaces;

    if (ai_mesh->mTangents)
    {
        rlMesh.tangents = (float*)malloc((sizeof(float) * rlMesh.vertexCount) * 4);
        unsigned int tangentCounter = 0;
        for (int j = 0; j < rlMesh.vertexCount * 4; j += 4)
        {
            rlMesh.tangents[j] = ai_mesh->mTangents[tangentCounter].x;
            rlMesh.tangents[j + 1] = ai_mesh->mTangents[tangentCounter].y;
            rlMesh.tangents[j + 2] = ai_mesh->mTangents[tangentCounter].z;
            rlMesh.tangents[j + 3] = 0;
            tangentCounter++;
        }
    }

    rlMesh.vboId = (unsigned int*)calloc(7, sizeof(unsigned int));
}

void AssimpConverter::CreateMesh(unsigned int flags, std::string outputPath, std::string format)
{
    Assimp::Importer impl;
    Assimp::Exporter exporter;
    this->scene = impl.ReadFile(this->FileName, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        printf("Error loading model: %s\n", impl.GetErrorString());

    if (!scene->HasMeshes())
        printf("The loaded scene has no meshes");

    for (int x = 0; x < scene->mNumMaterials; x++)
    {
        scene->mMaterials[x]->Clear();
    }

    if (exporter.Export(scene, format, outputPath) != AI_SUCCESS)
    {
        printf("Error exporting model: %s", exporter.GetErrorString());
    }
    else
    {
        printf("Model Exported!");
    }
}

RAYLIB::Mesh* AssimpConverter::GetMeshes()
{
    return this->temporalMesh;
}

unsigned int AssimpConverter::GetMeshCount()
{
    return (sizeof(this->temporalMesh) / sizeof(RAYLIB::Mesh));
}

void AssimpConverter::dealloc()
{
    delete this->temporalMesh;
    delete this;
}

int getAssimpExporters()
{
    return Assimp::Exporter().GetExportFormatCount();
}

const char* getAssimpExporterDescription(int a1)
{
    return Assimp::Exporter().GetExportFormatDescription(a1)->description;
}

const char* getAssimpExporterId(int a1)
{
    return Assimp::Exporter().GetExportFormatDescription(a1)->id;
}

#pragma managed(pop)