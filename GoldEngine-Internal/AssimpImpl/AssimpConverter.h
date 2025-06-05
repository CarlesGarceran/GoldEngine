#pragma managed(push, off)
#pragma once

#ifdef USE_ASSIMP

#include <assimp/scene.h>

extern std::string utilTool;

int getAssimpExporters();
const char* getAssimpExporterDescription(int);
const char* getAssimpExporterId(int a1);

public class AssimpConverter
{
private:
	std::string FileName;
	RAYLIB::Mesh* temporalMesh;
	const aiScene* scene;

	aiScene objectScene;

public:
	AssimpConverter(std::string, std::string, std::string);
	AssimpConverter(std::string, std::string);
	AssimpConverter(std::string, unsigned int, std::string);
	AssimpConverter(std::string, unsigned int, std::string, std::string);

	void CreateMesh(unsigned int, std::string, std::string);
	void CreateMesh(unsigned int, std::string);

	void ConvertToRaylibMesh();

	RAYLIB::Mesh* GetMeshes();
	unsigned int GetMeshCount();

	void dealloc();
};

#pragma managed(pop)

// MANAGED

namespace Engine::Native
{
	public ref class ManagedAssimpConverter
	{
	private:
		AssimpConverter* assimpConverter;

	public:
		ManagedAssimpConverter(String^, String^);
		ManagedAssimpConverter(String^, String^, String^);
		ManagedAssimpConverter(String^, unsigned int, String^);
		ManagedAssimpConverter(String^, unsigned int, String^, String^);

		void ConvertToRaylibMesh();

		RAYLIB::Mesh* GetMeshes();
		unsigned int GetMeshCount();

		void dealloc();
	};
}

#endif