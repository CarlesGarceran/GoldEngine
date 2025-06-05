#pragma once
#define WIN32_LEAN_AND_MEAN

// Coding Macros

#define Serialize Newtonsoft::Json::JsonConvert::SerializeObject
#define Deserialize Newtonsoft::Json::JsonConvert::DeserializeObject
#define JSON_SERIALIZE Newtonsoft::Json::JsonPropertyAttribute
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)
#define Engine_GCObject Engine::Internal::Components::GameObject^

// Preprocessor defines

#define LOGAPI_IMPL
#define RLIGHTS_IMPLEMENTATION

#define RLGL_SHOW_GL_DETAILS_INFO
#define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
#define GRAPHICS_API_OPENGL_33

// ENGINE CONFIGURATION

#define _CRT_SECURE_NO_WARNINGS
#define ENCRYPTION_PASSWORD "ChaosLanguage"

#define HIDE_CONSOLE false

#define MAX_LIGHTS 4

extern int max_lights;
extern unsigned int passwd;

#define ENGINE_VERSION "GoldEngine Ver: 0.6c"
#define EDITOR_VERSION "GoldEngine Editor Ver: 0.7a"
#define RUNTIME_VERSION "GoldEngine Runtime Prev-Ver: 0.3a"

#define USE_BULLET_PHYS
#define USE_ASSIMP

// PHYSICS SYSTEM

#ifdef _WIN64

#pragma comment(lib, "../Libs/x64/GFXLib.dll")
#pragma comment(lib, "../Libs/x64/GFXLib.lib")

#ifdef USE_ASSIMP

#pragma comment(lib, "../Libs/x64/assimp.lib")
#pragma comment(lib, "../Libs/x64/zlibstatic.lib")

#endif

#ifdef USE_BULLET_PHYS

#pragma comment(lib, "../Libs/x64/LinearMath.lib")
#pragma comment(lib, "../Libs/x64/BulletCollision.lib")
#pragma comment(lib, "../Libs/x64/BulletDynamics.lib")
#pragma comment(lib, "../Libs/x64/BulletInverseDynamics.lib")
#pragma comment(lib, "../Libs/x64/BulletSoftBody.lib")
#pragma comment(lib, "../Libs/x64/Bullet3Common.lib")
#pragma comment(lib, "../Libs/x64/Bullet3Dynamics.lib")
#pragma comment(lib, "../Libs/x64/Bullet3Geometry.lib")
#pragma comment(lib, "../Libs/x64/Bullet3Collision.lib")
#pragma comment(lib, "../Libs/x64/Bullet3OpenCL_clew.lib")
#pragma comment(lib, "../Libs/x64/Bullet2FileLoader.lib")

#endif

#else

#pragma comment(lib, "../Libs/x86/GFXLib.dll")
#pragma comment(lib, "../Libs/x86/GFXLib.lib")

#ifdef USE_ASSIMP

#pragma comment(lib, "../Libs/x86/assimp.lib")
#pragma comment(lib, "../Libs/x86/zlibstatic.lib")

#endif

#ifdef USE_BULLET_PHYS

#pragma comment(lib, "../Libs/x86/LinearMath.lib")
#pragma comment(lib, "../Libs/x86/BulletCollision.lib")
#pragma comment(lib, "../Libs/x86/BulletDynamics.lib")
#pragma comment(lib, "../Libs/x86/BulletInverseDynamics.lib")
#pragma comment(lib, "../Libs/x86/BulletSoftBody.lib")
#pragma comment(lib, "../Libs/x86/Bullet3Common.lib")
#pragma comment(lib, "../Libs/x86/Bullet3Dynamics.lib")
#pragma comment(lib, "../Libs/x86/Bullet3Geometry.lib")
#pragma comment(lib, "../Libs/x86/Bullet3Collision.lib")
#pragma comment(lib, "../Libs/x86/Bullet3OpenCL_clew.lib")
#pragma comment(lib, "../Libs/x86/Bullet2FileLoader.lib")

#endif

#endif