#pragma once

using namespace System;

/// <summary>
/// Allows for modifying and loading shaders at runtime.
/// THIS CLASS IS NOT PERSISTANT.
/// </summary>

[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
[Engine::Attributes::LuaAPIAttribute]
public ref class ShaderManager abstract
{
public:
	// load a shader into datapacks
	static void LoadShaderFromFile(unsigned int shaderId, String^ vsFp, String^ fsFp)
	{
		Engine::Assets::Storage::DataPacks::singleton().AddShader(shaderId, LoadShader(CastStringToNative(vsFp).c_str(), CastStringToNative(fsFp).c_str()));
	}

	// mainly used for pushing modified shader code into the datapacks
	static void LoadShaderFromMemory(unsigned int shaderId, String^ vertexShader, String^ fragmentShader)
	{
		Engine::Assets::Storage::DataPacks::singleton().AddShader(shaderId, ::LoadShaderFromMemory(CastStringToNative(vertexShader).c_str(), CastStringToNative(fragmentShader).c_str()));
	}

	// update a shader into datapacks \\

	static void UpdateShaderFromFile(unsigned int shaderId, String^ vsFp, String^ fsFp)
	{
		Engine::Assets::Storage::DataPacks::singleton().AddShader(shaderId, LoadShader(CastStringToNative(vsFp).c_str(), CastStringToNative(fsFp).c_str()));
	}

	static void UpdateShaderFromMemory(unsigned int shaderId, String^ vertexShader, String^ fragmentShader)
	{
		Engine::Assets::Storage::DataPacks::singleton().AddShader(shaderId, ::LoadShaderFromMemory(CastStringToNative(vertexShader).c_str(), CastStringToNative(fragmentShader).c_str()));
	}
};