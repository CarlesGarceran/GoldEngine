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


#ifdef USE_ASSIMP

#pragma managed(push, off)

Matrix ConvertAIMatrix4x4(aiMatrix4x4 mat)
{
    Matrix out;
    out.m0 = mat.a1; out.m4 = mat.a2; out.m8 = mat.a3; out.m12 = mat.a4;
    out.m1 = mat.b1; out.m5 = mat.b2; out.m9 = mat.b3; out.m13 = mat.b4;
    out.m2 = mat.c1; out.m6 = mat.c2; out.m10 = mat.c3; out.m14 = mat.c4;
    out.m3 = mat.d1; out.m7 = mat.d2; out.m11 = mat.d3; out.m15 = mat.d4;
    return out;
};

Vector3 ConvertAIVector3D(aiVector3D vert)
{
    Vector3 v = { 0 };
    v.x = vert.x;
    v.y = vert.y;
    v.z = vert.z;
    return v;
}

AssimpConverter::AssimpConverter(std::string fileName, std::string outputFile, std::string format) : AssimpConverter(fileName, aiProcess_Triangulate, outputFile, format)
{

}

AssimpConverter::AssimpConverter(std::string fileName, std::string format) : AssimpConverter(fileName, aiProcess_Triangulate, format)
{

}

AssimpConverter::AssimpConverter(std::string fileName) : AssimpConverter(fileName, "gltf")
{

}

AssimpConverter::AssimpConverter(std::string fileName, unsigned int flags, std::string format)
{
    this->FileName = fileName;
    this->scene = nullptr;

    this->CreateMesh(flags, format);
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

    //rlMesh.vboId = (unsigned int*)calloc(7, sizeof(unsigned int));
}

void AssimpConverter::CreateMesh(unsigned int flags, std::string format)
{
    Assimp::Importer impl;
    Assimp::Exporter exporter;
    this->scene = impl.ReadFile(this->FileName, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Error loading model: %s\n", impl.GetErrorString());
        return;
    }

    if (!scene->HasMeshes())
    {
        printf("The loaded scene has no meshes");
    }

    for (int x = 0; x < scene->mNumMaterials; x++)
    {
        scene->mMaterials[x]->Clear();
    }
}

void AssimpConverter::CreateMesh(unsigned int flags, std::string outputPath, std::string format)
{
    Assimp::Importer impl;
    Assimp::Exporter exporter;
    this->scene = impl.ReadFile(this->FileName, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Error loading model: %s\n", impl.GetErrorString());
        return;
    }

    if (!scene->HasMeshes())
    {
        printf("The loaded scene has no meshes");
    }

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

void AssimpConverter::ConvertToRaylibMesh()
{
    const unsigned int meshCount = this->scene->mNumMeshes;
    this->temporalMesh = new RAYLIB::Mesh[meshCount];
    this->meshCount = meshCount;

    for (int x = 0; x < meshCount; x++)
    {
        SetMeshData(this->temporalMesh[x], this->scene->mMeshes[x]);
    }
}

RAYLIB::Mesh* AssimpConverter::GetMeshes()
{
    return this->temporalMesh;
}

unsigned int AssimpConverter::GetMeshCount()
{
    return this->meshCount;
}

RAYLIB::Model& AssimpConverter::CreateModel()
{
    RAYLIB::Model model = {};

    if (scene == nullptr)
        return model;

    model.transform = RAYMATH::MatrixIdentity();
    
    // Load Textures
    aiTexture** textures = scene->mTextures;
    RAYLIB::Texture* rlTextures = new RAYLIB::Texture[scene->mNumTextures];

    for (int x = 0; x < scene->mNumTextures; x++)
    {
        aiTexture* texture = textures[x];

        if (texture->mHeight == 0) 
        {
            int size = texture->mWidth;
            unsigned char* data = (unsigned char*)texture->pcData;
            char ext[5] = ".png";
            if (texture->achFormatHint[0])
            {
                snprintf(ext, sizeof(ext), ".%s", texture->achFormatHint);
            }

            Image image = RAYLIB::LoadImageFromMemory(ext, data, size);
            Texture2D tex = LoadTextureFromImage(image);
            UnloadImage(image);

            rlTextures[x] = tex;
        }
        else
        {
            int width = texture->mWidth;
            int height = texture->mHeight;
            aiTexel* texels = texture->pcData;

            unsigned char* imageData = (unsigned char*)malloc(width * height * 4);
            for (int i = 0; i < width * height; i++) 
            {
                imageData[i * 4 + 0] = texels[i].r;
                imageData[i * 4 + 1] = texels[i].g;
                imageData[i * 4 + 2] = texels[i].b;
                imageData[i * 4 + 3] = texels[i].a;
            }

            Image image = {};
            image.data = imageData;
            image.width = width;
            image.height = height;
            image.mipmaps = 1;
            image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

            Texture2D tex = LoadTextureFromImage(image);
            UnloadImage(image);

            rlTextures[x] = tex;
        }
    }

    aiMaterial** materials = scene->mMaterials;
    RAYLIB::Material* rlMaterials = new RAYLIB::Material[scene->mNumMaterials];

    for (int x = 0; x < scene->mNumMaterials; x++)
    {
        aiMaterial* material = materials[x];

    }

    model.materials = rlMaterials;

    return model;
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

// MANAGED

Engine::Native::ManagedAssimpConverter::ManagedAssimpConverter(String^ fileName, String^ format) : ManagedAssimpConverter(fileName, aiProcess_Triangulate, format)
{

}

Engine::Native::ManagedAssimpConverter::ManagedAssimpConverter(String^ fileName, String^ output, String^ format) : ManagedAssimpConverter(fileName, aiProcess_Triangulate, output, format)
{

}

Engine::Native::ManagedAssimpConverter::ManagedAssimpConverter(String^ fileName, unsigned int flags, String^ format)
{
    assimpConverter = new AssimpConverter(CastStringToNative(fileName), flags, CastStringToNative(format));
}

Engine::Native::ManagedAssimpConverter::ManagedAssimpConverter(String^ fileName, unsigned int flags, String^ output, String^ format)
{
    assimpConverter = new AssimpConverter(CastStringToNative(fileName), flags, CastStringToNative(output), CastStringToNative(format));
}

RAYLIB::Mesh* Engine::Native::ManagedAssimpConverter::GetMeshes()
{
    return assimpConverter->GetMeshes();
}

unsigned int Engine::Native::ManagedAssimpConverter::GetMeshCount()
{
    return assimpConverter->GetMeshCount();
}

void Engine::Native::ManagedAssimpConverter::ConvertToRaylibMesh()
{
    assimpConverter->ConvertToRaylibMesh();
}

void Engine::Native::ManagedAssimpConverter::dealloc()
{
    assimpConverter->dealloc();

    delete assimpConverter;
}

#endif