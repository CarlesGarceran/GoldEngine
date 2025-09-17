// ATTRIBUTES \\

#include "Instantiable.h"
#include "ExecuteInEditModeAttribute.h"
#include "LuaAPI.h"

using namespace Engine::Attributes;

// DEPENDENCIES \\

#include "Reflection/ReflectedType.h"
#include "Reflection/ReflectableType.h"
#include "Includes.h"
#include "ManagedIncludes.h"
#include "Window.h"
#include "GlIncludes.h"
#include "LoggingAPI.h"
#include "DataManager.h"
#include "Cast.h"
#include "EngineConfig.h"
#include "Event.h"
#include "Object/Material.h"
#include "Object/Transform.h"
#include "Object/GameObject.h"
#include "CypherLib.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Raylib/include/rlImGui.h"
#include "DataPacks.h"
#include "DataPack.h"
#include "FileManager.h"
#include "EngineGC.h"
#include "imgui/FileExplorer/filedialog.h"


// INCLUDE ENGINE CLASSES \\

#include "SceneFormat.h"
#include "EngineIncludes.h"

#include "imguistyleserializer.h"

// Prefabs

#include "Object/Prefab.h"

// API

#include "Screen.h"
#include "Time.h"
#include "InputManager.h"
#include "ObjectManager.h"
#include "ShaderManager.h"
#include "LogFileReporter.h"
#include "AsmLoader.h"
#include "GraphicsWrapper.h"

// Daemons

#include "Objects/LightDm.h"

// Preload queue & scripts, can be used for loading/unloading certain data or doing operations with shaders, materials, models whatever.

#include "PreloadScript.h"


// lua virtual machine
#include "LuaVM.h"
#include "ManagedSignal.h"
// lua script object
#include "Objects/Script.h"
#include "Objects/LuaScript.h"

// Root datamodel

#include "Objects/Private/Scene.h"

// Geometry

#include "Objects/Abstract/Renderer.h"
#include "Objects/MeshRenderer/MeshRenderer.h"
#include "Objects/ModelRenderer/ModelRenderer.hpp"
#include "Objects/CapsuleRenderer.h"

// Audio

#include "Objects/Audio/AudioSource.h"

// Viepworts

#include "Objects/UI/RenderSurface3D.h"

// physics

#ifdef USE_BULLET_PHYS

#include "Objects/Physics/CollisionType.h"
#include "Objects/Physics/Native/NativePhysicsService.h"
#include "Objects/Physics/RigidBody.h"
#include "Objects/Physics/Triggers/Collider.h"
#include "Objects/Physics/PhysicsService.h"

#include "Objects/Physics/Colliders/BoxCollider.h"
#include "Objects/Physics/Colliders/CapsuleCollider.h"
#include "Objects/Physics/Colliders/MeshCollider.h"

#endif

// render pipelines

#include "Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "RenderPipelines/LitPBR_SRP.h"
#include "RenderPipelines/LightweightSRP.h"

// MiniAudio Init
#include "native/miniaudio.h"

#include "imfiledialog/ImFileDialog.h"

using namespace Engine;
using namespace Engine::EngineObjects;
using namespace Engine::EngineObjects::Native;
using namespace Engine::Internal;
using namespace Engine::Internal::Components;
using namespace Engine::Management;
using namespace Engine::Management::MiddleLevel;
using namespace Engine::Managers;
using namespace Engine::Scripting;

DataPacks dataPack;
unsigned int passwd = 0;
int max_lights = 4;

static System::Reflection::Assembly^ ResolveAssembly(System::Object^ sender, System::ResolveEventArgs^ args)
{
	System::Reflection::AssemblyName^ requestedName = gcnew System::Reflection::AssemblyName(args->Name);

	for each(System::Reflection::Assembly^ loadedAssembly in System::AppDomain::CurrentDomain->GetAssemblies())
	{
		if (System::Reflection::AssemblyName::ReferenceMatchesDefinition(gcnew System::Reflection::AssemblyName(loadedAssembly->FullName), requestedName))
			return loadedAssembly;
	}

	System::String^ assemblyFolder = System::IO::Path::Combine(AppDomain::CurrentDomain->BaseDirectory, "Bin/");
	System::String^ assemblyPath = System::IO::Path::Combine(assemblyFolder, requestedName->Name + ".dll");

	if (File::Exists(assemblyPath))
	{
		return System::Reflection::Assembly::LoadFrom(assemblyPath);
	}

	return nullptr;
}



static Newtonsoft::Json::JsonSerializerSettings^ SerializerSettings()
{
	auto serializerSettings = gcnew Newtonsoft::Json::JsonSerializerSettings();
	serializerSettings->Converters->Add(gcnew Newtonsoft::Json::Converters::StringEnumConverter());
	serializerSettings->Converters->Add(gcnew Newtonsoft::Json::Converters::KeyValuePairConverter());
	serializerSettings->TypeNameHandling = Newtonsoft::Json::TypeNameHandling::None;
	serializerSettings->PreserveReferencesHandling = PreserveReferencesHandling::Objects;

	return serializerSettings;
}

static void engine_keybinds()
{
	if (InputManager::IsKeyPressed(KeyCodes::KEY_F11))
		ToggleFullscreen();
}

static void SetupAssemblyResolver()
{
	AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&ResolveAssembly);
}

static void engine_bootstrap()
{
	SetupAssemblyResolver();
	Newtonsoft::Json::JsonConvert::DefaultSettings = gcnew Func<Newtonsoft::Json::JsonSerializerSettings^>(&SerializerSettings);

	if (!Directory::Exists("Bin/"))
		Directory::CreateDirectory("Bin");

	if (!Directory::Exists("Data"))
		Directory::CreateDirectory("Data");

	if (!Directory::Exists("Cfg"))
		Directory::CreateDirectory("Cfg");

	if (!Directory::Exists("Data/Keys/"))
		Directory::CreateDirectory("Data/Keys/");

	if (!File::Exists("Data/Keys/map.iv"))
	{
		String^ guid = System::Guid::NewGuid().ToString();
		File::WriteAllText("Data/Keys/map.iv", guid);
	}

	if (!Directory::Exists("Data/UserData/"))
		Directory::CreateDirectory("Data/UserData/");
}

std::string fileName = "Level0";

#if !defined(PRODUCTION_BUILD)

#pragma region EDITOR ENGINE

// EDITOR UI

// extra tools (converters, etc...)

#include "AssimpImpl/AssimpConverter.h"
#include "EditorGUI.h"

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

RAYLIB::Model mod;
float cameraSpeed = 1.25f;
bool controlCamera = true;
bool isOpen = true;
bool toggleControl = true;
bool initSettings = false;
bool styleEditor = false;
bool showCursor = true;
bool b1, b2, b3, b4, b5, b6, b7, b8, b9;
bool visualizeError;
bool readonlyLock = false;
bool fpsCap = true;
bool fpsCheck = true;
bool reparentLock = false;

std::string consoleBufferData = "";
std::string asset_filter_name = "";
std::string styleFN;

RAYLIB::Texture modelTexture;
RAYLIB::Texture textureTexture;
RAYLIB::Texture materialTexture;
RAYLIB::Texture scriptTexture;
RAYLIB::Texture soundTexture;

RAYLIB::Texture dragTexture;
RAYLIB::Texture translateTexture;
RAYLIB::Texture rotateTexture;
RAYLIB::Texture scaleTexture;

RAYLIB::Texture playTexture;
RAYLIB::Texture stopTexture;

bool fileDialogOpen = false;
int tmp1;
std::string password = "";
std::string packDataFileName = "";
int positionSelector = 0;

bool gameViewMode = false;

// error handling
char* errorReason;

bool ce1, ce2, ce3, ce4, ce5, ce6;

bool hierarchyVisible = true;
bool propertiesVisible = true;
bool assetsVisible = true;
bool consoleVisible = true;
bool scenevpVisible = true;
msclr::gcroot<String^> jsonData = "";
msclr::gcroot<String^> sceneSnapshot = "";

#include "EditorTools/CodeEditor.h"
#include "EditorTools/MaterialEditor.h"
#include "EditorWindow.h"

void CopyRaylibMatrixToFloat16(const RAYLIB::Matrix& mat, float out[16])
{
	out[0] = mat.m0;	out[1] = mat.m1;	out[2] = mat.m2;	out[3] = mat.m3;
	out[4] = mat.m4;	out[5] = mat.m5;	out[6] = mat.m6;	out[7] = mat.m7;
	out[8] = mat.m8;	out[9] = mat.m9;	out[10] = mat.m10;	out[11] = mat.m11;
	out[12] = mat.m12;	out[13] = mat.m13;	out[14] = mat.m14;	out[15] = mat.m15;
}

typedef enum assetDisplay
{
	ALL,
	MODELS,
	TEXTURES,
	SOUND,
	MUSIC,
	SCRIPTS,
	PREFAB,
	MATERIALS
};

const std::vector<std::string> convertableFiles = {
	".fbx",
	".vrm",
	".mesh",
	".stl",
};

assetDisplay displayingAssets;

int displayingAsset = 0;

UNMANAGED_BEGIN

void DeleteTexture(void* tex)
{
	if (tex == nullptr) return;

	Texture2D* texture = reinterpret_cast<Texture2D*>(tex);
	UnloadTexture(*texture);
	delete texture;
}

void* CreateTexture(RAYLIB::Image* image, int w, int h, char fmt)
{
	if (image == nullptr || image->data == nullptr) return nullptr;

	if (w != image->width || h != image->height) {
	}

	if (fmt == 0)
	{
		int pixelCount = image->width * image->height;
		RAYLIB::Color* pixels = reinterpret_cast<RAYLIB::Color*>(image->data);

		for (int i = 0; i < pixelCount; i++)
		{
			std::swap(pixels[i].r, pixels[i].b);
		}
	}

	Texture2D* tex = new Texture2D;
	*tex = LoadTextureFromImage(*image);
	
	return reinterpret_cast<void*>(tex);
}

void ConfigureImFileDialog()
{
	ifd::FileDialog::Instance().CreateTexture = &CreateTexture;
	ifd::FileDialog::Instance().DeleteTexture = &DeleteTexture;
}

UNMANAGED_END

void ExecuteConsoleCommand(EditorWindow^ windowPtr, std::string consoleCommand)
{
	if (
		consoleCommand.find("help()") != std::string::npos ||
		consoleCommand.find("Help()") != std::string::npos ||
		consoleCommand.find("cmds()") != std::string::npos ||
		consoleCommand.find("Cmds()") != std::string::npos)
	{
		printConsole("Console Commands (case-sensitive):");
		printConsole("clear() | Clear() - Clears the console");
		printConsole("help() | Help() | cmds() | Cmds() - Shows this message");
		printConsole("Anything else will be ran in the internal LUAVM of the GameEngine.");

	}
	if (consoleCommand.find("clear()") != std::string::npos || consoleCommand.find("Clear()") != std::string::npos)
	{
		Engine::Scripting::Logging::clearLogs();
	}
	else if (consoleCommand.find("ifd()") != std::string::npos)
	{
		ifd::FileDialog::Instance().Open("TestFileDialog", "Open file", "All Files (*.*),.*", false);
	}
	else
	{
		try
		{
			windowPtr->getLuaVM()->ClearGlobals();
			windowPtr->getLuaVM()->RegisterGlobalFunctions();

			windowPtr->getLuaVM()->RegisterScript(gcnew String(consoleCommand.c_str()));
		}
		catch (Exception^ ex)
		{
			printError("Failed executing script: " + ex);
			printError("Do you want to see the help? type help()");
		}
	}
}
void ThrowUIError(String^ eR)
{
	std::string convErrRes = CastStringToNative(eR);

	errorReason = new char[convErrRes.size()];

	strcpy(errorReason, convErrRes.c_str());

	visualizeError = true;
}
void ShowError()
{
	if (ImGui::BeginPopupModal("Unexpected Error", (bool*)false, ImGuiWindowFlags_NoResize))
	{
		ImGui::Text(errorReason);

		if (ImGui::Button("Accept"))
		{
			visualizeError = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void SaveToFile(String^ filePath)
{
	File::WriteAllText(filePath, jsonData);
	jsonData = "";
}
String^ GetParentRoute(Engine::Internal::Components::Transform^ transform)
{
	if (transform != nullptr && transform->parent != nullptr)
	{
		auto parentInstance = Singleton<ObjectManager^>::Instance->GetObjectByUid(transform->parent->GetUID());
		return GetParentRoute(parentInstance->transform) + "/" + parentInstance->name;
	}
	else
		return "World";
}
String^ GetAccessRoute(Engine::Internal::Components::GameObject^ object)
{
	return GetParentRoute(object->transform) + "/" + object->name;
}

void TogglePlayMode(EditorWindow^ window)
{
	bool playmode = !EngineState::PlayMode;

	auto scene = Singleton<Engine::Scripting::ObjectManager^>::Instance->GetLoadedScene();

	if (playmode)
	{
		sceneSnapshot = Serialize(scene);
		print("[GoldEngine]:", "Entering PlayMode");
	}
	else
	{
		print("[GoldEngine]:", "Leaving PlayMode");
	}

	EngineState::PlayMode = playmode;
}

EditorWindow::EditorWindow()
{
#if HIDE_CONSOLE == true
	WinAPI::FreeCons();
#endif

	codeEditor = gcnew CodeEditor(this);
	materialEditor = gcnew MaterialEditor(this);

	password = ENCRYPTION_PASSWORD;

	engine_bootstrap();

	dataPack = DataPacks();

	assemblies = gcnew System::Collections::Generic::List<EngineAssembly^>();

	assemblies->Add(gcnew EngineAssembly(System::Reflection::Assembly::GetExecutingAssembly()));

	for each (String ^ fileName in Directory::GetFiles("Bin\\"))
	{
		if (fileName->Contains(".dll"))
		{
			String^ path = Directory::GetCurrentDirectory() + "\\" + fileName;

			assemblies->Add(gcnew EngineAssembly(path));
		}
	}

	SceneManager::SetAssemblyManager(assemblies);
	luaVM = gcnew Engine::Lua::VM::LuaVM();

	for each (EngineAssembly ^ assembly in assemblies)
	{
		assembly->ListAssemblyTypes();
	}

	Start();
}
void EditorWindow::SpecializedPropertyEditor(Engine::Internal::Components::GameObject^ object)
{
	if (object != nullptr)
	{
		auto type = object->GetObjectType();

		switch (type)
		{
		case ObjectType::CubeRenderer:
		{
			Engine::EngineObjects::CubeRenderer^ renderer = Cast::Dynamic<Engine::EngineObjects::CubeRenderer^>(object);

			unsigned int tint = renderer->color;

			auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(tint));

			float rawData[4] =
			{
				float4.x,
				float4.y,
				float4.z,
				float4.w
			};

			ImGui::Text("Tint Editor: ");
			ImGui::SameLine();
			if (ImGui::ColorPicker4("###TINT_SETTER", rawData))
			{
				renderer->SetColor(ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3])));
			}

		}
		break;

		case ObjectType::LightSource:
		{
#if USE_ILLUMINA
			Engine::EngineObjects::LightSource^ light = Cast::Dynamic<Engine::EngineObjects::LightSource^>(object);
			bool lightEnabled = light->enabled;
			ImGui::SeparatorText("Light Source");
			ImGui::Text("Enabled: ");
			ImGui::SameLine();
			if (ImGui::Checkbox("###LIGHT_ENABLED", &lightEnabled))
			{
				if (lightEnabled)
				{
					light->enabled = true;
				}
				else
				{
					light->enabled = false;
				}
			}

			int lightType = light->lightType;
			ImGui::Text("Light Type:");
			ImGui::SameLine();
			const char* data[] = { "Directional Light", "Point Light", "Spot Light" };

			if (ImGui::Combo("###LIGHT_TYPE", &lightType, data, IM_ARRAYSIZE(data)))
			{
				light->lightType = (rPBR::LightType)lightType;
			}

			float intensity = light->intensity;
			ImGui::Text("Radius:");
			ImGui::SameLine();
			if (ImGui::DragFloat("###LIGHT_INTENSITY", &intensity, 0.25f, 0.1f, float::MaxValue, "%.1f"))
			{
				light->intensity = intensity;
			}
			int shaderId = light->shaderId;
			ImGui::Text("Shader Id:");
			ImGui::SameLine();
			if (ImGui::InputInt("###SHADER_ID", &shaderId, 1, 1))
			{
				light->shaderId = shaderId;
			}

			float lightPower = light->lightPower;
			ImGui::Text("Light Intensity:");
			ImGui::SameLine();

			if (ImGui::DragFloat("###LIGHT_POWER", &lightPower, 100.0f, float::MinValue, float::MaxValue, "%.1f"))
			{
				light->lightPower = lightPower;
			}

			if ((rPBR::LightType)lightType == rPBR::LightType::LIGHT_DIRECTIONAL || (rPBR::LightType)lightType == rPBR::LightType::LIGHT_SPOT)
			{
				Engine::Components::Vector3 target = light->target;

				float nativeVector[3] = { light->target->x, light->target->y, light->target->z };

				ImGui::Text("Target: ");
				ImGui::SameLine();
				if (ImGui::DragFloat2("###LIGHT_TARGET", nativeVector, 1.0f, float::MinValue, float::MaxValue, "%.3f"))
				{
					light->target = Engine::Components::Vector3(nativeVector[0], nativeVector[1], nativeVector[2]);
				}
			}

			if ((rPBR::LightType)lightType == rPBR::LightType::LIGHT_SPOT)
			{
				float cutoff = light->cutoff;
				ImGui::Text("Cutoff:");
				ImGui::SameLine();
				if (ImGui::DragFloat("###LIGHT_CUTOFF", &cutoff, 0.5f, 0.1f, float::MaxValue, "%.1f"))
				{
					light->cutoff = cutoff;
				}


				float outercutoff = light->outerCutoff;
				ImGui::Text("Outer Cutoff:");
				ImGui::SameLine();
				if (ImGui::DragFloat("###LIGHT_OUTERCUTOFF", &outercutoff, 0.5f, 0.1f, float::MaxValue, "%.1f"))
				{
					light->outerCutoff = outercutoff;
				}
			}

			auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(light->lightColor));

			float rawData[4] =
			{
				float4.x,
				float4.y,
				float4.z,
				float4.w
			};

			ImGui::Text("Tint Editor: ");
			ImGui::SameLine();
			if (ImGui::ColorPicker4("###TINT_SETTER", rawData))
			{
				light->lightColor = (ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3])));
			}
#endif
		}
		break;

		case ObjectType::Script:
		{
			Engine::EngineObjects::ScriptBehaviour^ script = (Engine::EngineObjects::ScriptBehaviour^)object;

			if (script->assemblyReference->Contains("LuaScript"))
			{
				auto scr = (Engine::EngineObjects::LuaScript^)script;

				ImGui::SeparatorText("Linked Source");

				ImGui::Text("Binary file path: ");
				ImGui::SameLine();

				std::string nativePath = CastStringToNative(scr->luaFilePath);

				char* data = nativePath.data();

				if (ImGui::InputText("###LUA_LINKEDSOURCE",
					data, scr->luaFilePath->Length + (8 * 32), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					scr->luaFilePath = gcnew String(data);
				}

				if (ImGui::Button("Reload lua source"))
				{
					scr->Reset();
				}
			}

			ImGui::SeparatorText("Attributes");
			if (ImGui::BeginListBox("###ATTRIBUTE_LISTBOX", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() - 240 }))
			{
				int idx = 0;
				for each (Engine::Scripting::Attribute ^ attrib in script->attributes->attributes)
				{
					if (attrib != nullptr)
					{
						if (attrib->getValueType() == nullptr)
							continue;

						ImGui::Text(CastStringToNative(attrib->accessLevel.ToString() + " | " + attrib->name + " (" + attrib->getValueType()->Name + ")").c_str());
						if (attrib->getValueType()->Equals(String::typeid))
						{
							StringEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(System::Enum::typeid) || attrib->getValueType()->IsSubclassOf(System::Enum::typeid))
						{
							EnumEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(Engine::Components::Vector3::typeid))
						{
							Vector3Editor(attrib);
						}
						else if (attrib->getValueType()->Equals(Engine::Components::Vector2::typeid))
						{
							Vector2Editor(attrib);
						}
						else if (attrib->getValueType()->Equals(UInt32::typeid))
						{
							int value = (unsigned int)attrib->getValue();

							if (ImGui::InputInt(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 1, 1))
							{
								attrib->setValue(gcnew UInt32(value), false);
								attrib->setType(UInt32::typeid);
							}
						}
						else if (attrib->getValueType()->Equals(Int32::typeid))
						{
							IntegerEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(Int64::typeid))
						{
							long long tmp = (Int64)attrib->getValue();

							int value = (int)tmp;

							if (ImGui::InputInt(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 1, 1))
							{
								attrib->setValue(gcnew Int64(value), false);
								attrib->setType(Int64::typeid);
							}
						}
						else if (attrib->getValueType()->Equals(float::typeid))
						{
							float tmp = (float)attrib->getValue();

							float value = (float)tmp;

							if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 0.1f, 0.5f, "%.2f"))
							{
								attrib->setValue(value, false);
								attrib->setType(float::typeid);
							}
						}
						else if (attrib->getValueType()->Equals(Single::typeid))
						{
							float tmp = (float)attrib->getValue();

							float value = (float)tmp;

							if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 0.1f, 0.5f, "%.2f"))
							{
								attrib->setValue(value, false);
								attrib->setType(float::typeid);
							}
						}
						else if (attrib->getValueType()->Equals(Engine::Internal::Components::GameObject::typeid) || attrib->getValueType()->IsSubclassOf(Engine::Internal::Components::GameObject::typeid) || attrib->getValueType()->IsSubclassOf(Engine::EngineObjects::ScriptBehaviour::typeid) || attrib->getValueType()->IsSubclassOf(Engine::EngineObjects::Script::typeid))
						{
							std::string temp = std::string("");
							if (attrib->getValue() == nullptr)
							{
								temp = CastStringToNative("NOT ASSIGNED - (NULL)###" + idx);
							}
							else
							{
								Engine::Internal::Components::GameObject^ value = attrib->getValueAs<Engine::Internal::Components::GameObject^>();

								temp = CastStringToNative(value->name + " - (" + GetAccessRoute(value) + ")###" + idx);
							}

							if (ImGui::Button(temp.c_str()))
							{
								if (!selectionLock)
									selectionLock = true;
								else
								{
									if (selectionObject->GetType() == attrib->getValueType() || attrib->getValueType()->IsAssignableFrom(selectionObject->GetType()))
									{
										selectionLock = false;
										attrib->setValue(selectionObject, false);
									}
									else
									{
										selectionLock = false;
										attrib->setValue(nullptr, false);
									}
								}
							}
						}
						else if (attrib->getValueType()->Equals(Double::typeid))
						{
							DoubleEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(bool::typeid))
						{
							BoolEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(Engine::Components::Color::typeid))
						{
							ColorEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(System::Collections::Generic::List::typeid))
						{
							ListEditor(attrib);
						}

						idx++;

						ImGui::Separator();
					}
				}

				ImGui::EndListBox();
			}
		}

		break;

		}
	}
}
Engine::Lua::VM::LuaVM^ EditorWindow::getLuaVM()
{
	return luaVM;
}
void EditorWindow::OpenFileExplorer(std::string name, Engine::Editor::Gui::explorerMode mode, Engine::Editor::Gui::onFileSelected^ callback)
{
	fileExplorer->SetWindowName(name);
	fileExplorer->setExplorerMode(mode);
	fileExplorer->Open();

	fileExplorer->OnCompleted(callback);
}
void EditorWindow::DrawHierarchyInherits(Engine::Management::Scene^ scene, Engine::Internal::Components::GameObject^ parent, int depth)
{
	for (int x = 0; x < scene->GetRenderQueue()->Count; x++)
	{
		Engine::Internal::Components::GameObject^ _obj = scene->GetRenderQueue()[x];

		auto _reference = _obj;
		auto _type = _obj->GetObjectType();

		if (parent->Equals(_reference))
			continue;

		if (_reference->getTransform() == nullptr)
			continue;

		if (_reference->getTransform()->parent != nullptr)
		{
			if (_reference->getTransform()->parent->GetUID() == parent->getTransform()->GetUID())
			{
				String^ refName = "";

				refName += _reference->name;
				refName += (_reference->active == true) ? "" : "(INACTIVE)";

				if (_reference->GetChildren()->Count > 0)
				{
					bool isOpen = ImGui::TreeNodeEx(CastStringToNative("##_" + refName + "_(ENGINE_PROTECTED)_" + "###_" + _reference->getTransform()->GetUID() + x).c_str());
					ImGui::SameLine();

					if (_type == ObjectType::Daemon || _type == ObjectType::Datamodel || _type == ObjectType::LightManager || _reference->isProtected())
					{
						if (ImGui::Selectable(CastStringToNative(refName + " (ENGINE PROTECTED)" + "###" + _reference->getTransform()->GetUID() + "_" + (depth + x)).c_str()))
						{
							if (reparentLock)
								reparentObject = _reference;
							else if (selectionLock)
								selectionObject = _reference;
							else
							{
								readonlyLock = true;
								selectedObject = _reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}
					else
					{
						if (ImGui::Selectable(CastStringToNative(refName + "###" + _reference->getTransform()->GetUID() + "_" + (depth + x)).c_str()))
						{
							if (reparentLock)
								reparentObject = _reference;
							else if (selectionLock)
								selectionObject = _reference;
							else
							{
								readonlyLock = false;
								selectedObject = _reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}

					if (isOpen)
					{
						DrawHierarchyInherits(scene, _reference, depth + 1);
						ImGui::TreePop();
					}
				}
				else
				{
					bool isOpen = ImGui::TreeNodeEx(CastStringToNative("##_" + refName + "_(ENGINE_PROTECTED)_" + "###_" + _reference->getTransform()->GetUID() + x).c_str(), ImGuiTreeNodeFlags_Leaf);
					ImGui::SameLine();

					if (_type == ObjectType::Daemon || _type == ObjectType::Datamodel || _type == ObjectType::LightManager || _reference->isProtected())
					{
						if (ImGui::Selectable(CastStringToNative(refName + " (ENGINE PROTECTED)" + "###" + _reference->getTransform()->GetUID() + "_" + (depth + x)).c_str()))
						{
							if (reparentLock)
								reparentObject = _reference;
							else if (selectionLock)
								selectionObject = _reference;
							else
							{
								readonlyLock = true;
								selectedObject = _reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}
					else
					{
						if (ImGui::Selectable(CastStringToNative(refName + "###" + _reference->getTransform()->GetUID() + "_" + (depth + x)).c_str()))
						{
							if (reparentLock)
								reparentObject = _reference;
							else if (selectionLock)
								selectionObject = _reference;
							else
							{
								readonlyLock = false;
								selectedObject = _reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}

					if (isOpen)
					{
						ImGui::TreePop();
					}
				}

			}
		}
	}
}
void EditorWindow::createAssetEntries(String^ path)
{
	for each (String ^ f in Directory::GetFiles(path))
	{
		f = f->Replace(R"(\)", "/");
		array<String^>^ tmp = f->Split('/');
		auto t = tmp[tmp->Length - 1] + "\n";

		bool rendered = false;

		if (asset_filter_name != "")
			if (CastStringToNative(t).find(asset_filter_name) == std::string::npos)
				continue;

		if ((f->Contains(".obj") || f->Contains(".glb") || f->Contains(".gltf") || f->Contains(".vox")) && (displayingAssets == ALL || displayingAssets == MODELS)) // model types
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &modelTexture))
			{
				unsigned int assetId = 0;
				auto res = packedData->hasAsset(Engine::Assets::Management::assetType::_Model, f);
				if (!std::get<0>(res))
				{
					assetId = packedData->GetAssetID(Engine::Assets::Management::assetType::_Model);

					packedData->AddModel(assetId, f);

					packedData->WriteToFile(packedData->getFile(), passwd);
				}
				else
				{
					assetId = std::get<1>(res);
				}

				auto meshRenderer = gcnew Engine::EngineObjects::Geometry::ModelRenderer(
					"ModelRenderer",
					gcnew Engine::Internal::Components::Transform(
						Engine::Components::Vector3(0, 0, 0),
						Engine::Components::Vector3(0, 0, 0),
						Engine::Components::Vector3(1, 1, 1),
						nullptr
					),
					assetId,
					0
				);
				meshRenderer->SetParent(scene->GetDatamodelMember("workspace"));
				scene->AddObjectToScene(meshRenderer);
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());

			rendered = true;
		}

		for (std::string meshExt : convertableFiles)
		{
			if ((f->Contains(gcnew String(meshExt.c_str()))) && (displayingAssets == ALL || displayingAssets == MODELS))
			{
				if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &modelTexture))
				{
					EnableFBXConverter(CastStringToNative(f));
				}
				ImGui::SameLine();
				ImGui::Text(CastStringToNative(t).c_str());
			}
		}

		if ((f->Contains(".ogg") || f->Contains(".mp3") || f->Contains(".wav")) && (displayingAssets == ALL || displayingAssets == SOUND))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t + " - As Sound").c_str(), &soundTexture))
			{
				unsigned int assetId = 0;
				auto res = packedData->hasAsset(Engine::Assets::Management::assetType::_Sound, f);
				if (!std::get<0>(res))
				{
					assetId = packedData->GetAssetID(Engine::Assets::Management::assetType::_Sound);

					packedData->AddSound(assetId, f);

					packedData->WriteToFile(packedData->getFile(), passwd);
				}
				else
				{
					assetId = std::get<1>(res);
				}
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if ((f->Contains(".ogg") || f->Contains(".mp3") || f->Contains(".wav")) && (displayingAssets == ALL || displayingAssets == MUSIC))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t + " - As Music").c_str(), &materialTexture))
			{
				unsigned int assetId = 0;
				auto res = packedData->hasAsset(Engine::Assets::Management::assetType::_Musics, f);
				if (!std::get<0>(res))
				{
					assetId = packedData->GetAssetID(Engine::Assets::Management::assetType::_Musics);

					packedData->AddMusic(assetId, f);

					packedData->WriteToFile(packedData->getFile(), passwd);
				}
				else
				{
					assetId = std::get<1>(res);
				}
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if (f->Contains(".lua") && (displayingAssets == ALL || displayingAssets == SCRIPTS))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &scriptTexture))
			{
				codeEditor->createTab(f, TextEditor::LanguageDefinition::Lua());
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if (f->Contains(".prefab") && (displayingAssets == ALL || displayingAssets == PREFAB))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &modelTexture))
			{
				auto scriptData = Prefab::LoadPrefab(f);

				for each (GameObject ^ d in scriptData)
				{
					scene->AddObjectToScene(d);
				}
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if (((f->Contains(".png") || f->Contains(".jpg") || f->Contains(".bmp") || f->Contains(".dds") || f->Contains(".hdr"))) && (displayingAssets == ALL || displayingAssets == TEXTURES))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &textureTexture))
			{
				unsigned int assetId = 0;
				auto res = packedData->hasAsset(Engine::Assets::Management::assetType::_Texture2D, f);
				if (!std::get<0>(res))
				{
					assetId = packedData->GetAssetID(Engine::Assets::Management::assetType::_Texture2D);

					packedData->AddTextures2D(assetId, f);

					packedData->WriteToFile(packedData->getFile(), passwd);
				}
				else
				{
					assetId = std::get<1>(res);
				}
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if (((f->Contains(".mat")) || f->Contains(".material")) && (displayingAssets == ALL || displayingAssets == MATERIALS))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &materialTexture))
			{
				unsigned int assetId = 0;
				auto res = packedData->hasAsset(Engine::Assets::Management::assetType::_Material, f);
				if (!std::get<0>(res))
				{
					assetId = packedData->GetAssetID(Engine::Assets::Management::assetType::_Material);

					packedData->AddMaterial(assetId, f); 

					packedData->WriteToFile(packedData->getFile(), passwd);
				}
				else
				{
					assetId = std::get<1>(res);
				}
	
				materialEditor->SetMaterial(assetId);
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}
	}

	for each (String ^ dir in Directory::GetDirectories(path))
	{
		createAssetEntries(dir);
	}
}
void EditorWindow::DrawConsole()
{
	if (consoleVisible && ImGui::Begin("Console", &consoleVisible))
	{
		if (ImGui::Button("Clear"))
		{
			Engine::Scripting::Logging::clearLogs();
		}

		const ImVec2 windowSize = ImGui::GetWindowSize();

		if (ImGui::BeginListBox("###CONSOLE_OUTPUT", { windowSize.x - 20, windowSize.y - 80 }))
		{
			for each (Engine::Scripting::Log ^ log in Engine::Scripting::Logging::getLogs())
			{
				switch (log->logType)
				{
				case TraceLogLevel::LOG_INFO:
					ImGui::TextColored({ 1.0f,1.0f, 1.0f, 1.0f }, CastStringToNative(log->message).c_str());
					break;
				case TraceLogLevel::LOG_DEBUG:
					ImGui::TextColored({ 0.141f, 0.851f, 0.929f, 1.0f }, CastStringToNative(log->message).c_str());
					break;
				case TraceLogLevel::LOG_FATAL:
					ImGui::TextColored({ 0,0,0,1.0f }, CastStringToNative(log->message).c_str());
					break;
				case TraceLogLevel::LOG_ERROR:
					ImGui::TextColored({ 1.0f,0,0,1.0f }, CastStringToNative(log->message).c_str());
					break;
				case TraceLogLevel::LOG_WARNING:
					ImGui::TextColored({ 1.0f, 0.533f, 0.0f, 1.0f }, CastStringToNative(log->message).c_str());
					break;
				}
			}

			ImGui::EndListBox();
		}

		ImGui::Text("Console Commands:");
		ImGui::SameLine();
		if (ImGui::InputText("###CONSOLE_COMMANDS", &consoleBufferData, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			ExecuteConsoleCommand(this, consoleBufferData);
			consoleBufferData = "";
		}

	}

	if (consoleVisible)
		ImGui::End();
}
void EditorWindow::PackData(String^ convertedData)
{
	if (!convertedData->Contains("."))
	{
		ImGui::CloseCurrentPopup();
		ThrowUIError("File does not have extension");
		ImGui::EndPopup();
		return;
	}
	else if (convertedData->Length <= 0)
	{
		ImGui::CloseCurrentPopup();
		ThrowUIError("File length is not valid");
		ImGui::EndPopup();
		return;
	}
	else if (convertedData->IsNullOrEmpty(convertedData))
	{
		ImGui::CloseCurrentPopup();
		ThrowUIError("FileName is not valid");
		ImGui::EndPopup();
		return;
	}

	print("[Asset Packer]: ", "------------------------");
	print("[Asset Packer]: ", "Packing data to -> " + convertedData);

	for each (auto asset in loadedAssets)
	{
		print("[Asset Packer]: ", "Packing asset -> " + asset);
	}

	print("[Asset Packer]: ", "------------------------");

	FileManager::WriteToCustomFile(convertedData, gcnew String(password.c_str()), loadedAssets->ToArray());
}
void EditorWindow::Start()
{
	SetTraceLogLevel(LOG_DEBUG);
	SetWindowFlags(FLAG_INTERLACED_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
	OpenWindow(1280, 720, (const char*)EDITOR_VERSION);

	ConfigureImFileDialog();
	gcnew Engine::Managers::SignalManager();

	if (File::Exists("./Data/Keys/secrets.dat"))
	{
		auto secrets = Engine::Config::EngineSecrets::ImportSecrets("./Data/Keys/secrets.dat");

		unsigned int password = Engine::Encryption::CypherLib::GetPasswordBytes(secrets->getPassword());
		passwd = password;

		auto config = Engine::Config::EngineConfiguration::ImportConfig("./Data/appinfo.dat", password);

		if (!Directory::Exists(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/'))))
		{
			Directory::CreateDirectory(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/')));
		}

		gcnew Engine::Utils::LogReporter(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath);
	}
	else
	{
		auto secrets = gcnew Engine::Config::EngineSecrets(ENCRYPTION_PASSWORD);

		secrets->ExportSecrets("./Data/Keys/secrets.dat");

		auto config = gcnew Engine::Config::EngineConfiguration();
		config->ExportConfig("./Data/appinfo.dat");

		if (!Directory::Exists(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/'))))
		{
			Directory::CreateDirectory(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/')));
		}

		gcnew Engine::Utils::LogReporter(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath);
	}

	LayerManager::RegisterDefaultLayers();

	Preload();

	Loop();
}
void EditorWindow::ExecAsIdentifiedObject(Engine::Internal::Components::ObjectType type, System::Object^ object)
{
	if (scene->sceneLoaded())
	{
		Engine::Internal::Components::GameObject^ modelRenderer = (Engine::Internal::Components::GameObject^)object;
		modelRenderer->GameDraw();
		modelRenderer->GameDrawGizmos();
	}
}
void EditorWindow::DrawMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine", true))
		{
			ImGui::SeparatorText("Encryption");

			if (ImGui::BeginMenu("Set encryption password"))
			{
				if (ImGui::InputText("###PASSWORD_SETTER", &password, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					Engine::Config::EngineSecrets::singleton()->setEncryptionPassword(gcnew String(password.c_str()));
					passwd = Engine::Encryption::CypherLib::GetPasswordBytes(Engine::Config::EngineSecrets::singleton()->getPassword());
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Export Secrets"))
			{
				Engine::Config::EngineSecrets::singleton()->ExportSecrets("./Data/Keys/secrets.dat");
			}

			ImGui::SeparatorText("Project Startup Settings");

			if (ImGui::BeginMenu("Engine Configuration"))
			{
				if (ImGui::MenuItem("Edit Engine Configuration"))
				{
					b9 = true;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Export"))
				{
					Engine::Config::EngineConfiguration::singleton()->ExportConfig("./Data/appinfo.dat");
				}

				ImGui::EndMenu();
			}

			ImGui::SeparatorText("Project");

			if (ImGui::BeginMenu("Generate project"))
			{
				if (ImGui::MenuItem("C++ Template"))
				{

				}

				if (ImGui::MenuItem("C# Template"))
				{

				}

				ImGui::EndMenu();
			}	

			if (ImGui::MenuItem("Build"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				Exit();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Scene", true))
		{
			if (ImGui::MenuItem("New Scene", "", false, true))
			{
				b1 = true;
			}
			if (ImGui::MenuItem("Open Scene", "", false, true))
			{
				b3 = true;
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				SceneManager::SaveSceneToFile(scene, passwd);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit", true))
		{
			ImGui::SeparatorText("AssetPacks");
			if (ImGui::MenuItem("AssetPack Editor"))
			{
				b4 = true;
			}
			ImGui::SeparatorText("Engine");
			if (ImGui::MenuItem("Layer Editor"))
			{
				b8 = true;
			}

			if (ImGui::MenuItem("Material Editor"))
			{
				materialEditor->ShowGUI();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Views", true))
		{
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Hierarchy", "", hierarchyVisible))
					if (!hierarchyVisible)
						hierarchyVisible = true;
					else
						hierarchyVisible = false;

				if (ImGui::MenuItem("Properties", "", propertiesVisible))
					if (!propertiesVisible)
						propertiesVisible = true;
					else
						propertiesVisible = false;

				if (ImGui::MenuItem("Assets", "", assetsVisible))
					if (!assetsVisible)
						assetsVisible = true;
					else
						assetsVisible = false;

				if (ImGui::MenuItem("Scene Viewport", "", scenevpVisible))
					if (!scenevpVisible)
						scenevpVisible = true;
					else
						scenevpVisible = false;

				if (ImGui::MenuItem("Console", "", consoleVisible))
					if (!consoleVisible)
						consoleVisible = true;
					else
						consoleVisible = false;

				ImGui::EndMenu();
			}

			ImGui::SeparatorText("Modes");

			if (ImGui::MenuItem("Editor View"))
			{
				TogglePlayMode(this);
			}
			if (ImGui::MenuItem("Game View"))
			{
				TogglePlayMode(this);
			}
			ImGui::SeparatorText("Render Pipelines");

			if (ImGui::BeginMenu("Pipelines"))
			{
				for each (EngineAssembly ^ assembly in assemblies)
				{
					auto types = assembly->GetTypesOf(Engine::Render::ScriptableRenderPipeline::typeid);

					for each (System::Type ^ pipeline in types)
					{
						if (ImGui::MenuItem(CastStringToNative(pipeline->Name).c_str()))
						{
							if (renderPipeline != nullptr)
								renderPipeline->OnUnloadPipeline();

							renderPipeline = (Engine::Render::ScriptableRenderPipeline^)assembly->CreateSimple(pipeline);
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Object", true))
		{
			Engine::Internal::Components::Transform^ parent = nullptr;

			if (selectedObject != nullptr)
				parent = selectedObject->transform;

			if (ImGui::MenuItem("Empty Object"))
			{
				Engine::EngineObjects::Script^ newObject = gcnew Engine::EngineObjects::Script("Empty Object",
					gcnew Engine::Internal::Components::Transform(
						Engine::Components::Vector3::create({ 0,0,0 }),
						Engine::Components::Vector3::create({ 0,0,0 }),
						Engine::Components::Vector3::create({ 1,1,1 }),
						parent
					)
				);

				scene->AddObjectToScene(newObject);
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Cube Renderer"))
				{
					Engine::EngineObjects::CubeRenderer^ cubeRenderer = gcnew Engine::EngineObjects::CubeRenderer("CubeRenderer",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							parent
						), 0xFFFFFFFF);

					scene->AddObjectToScene(cubeRenderer);
				}

				if (ImGui::MenuItem("Capsule Renderer"))
				{
					Engine::EngineObjects::Geometry::CapsuleRenderer^ cubeRenderer = gcnew Engine::EngineObjects::Geometry::CapsuleRenderer(
						"CapsuleRenderer",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							parent
						)
					);

					scene->AddObjectToScene(cubeRenderer);
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Cameras"))
			{
				if (ImGui::MenuItem("Camera3D"))
				{
					Engine::EngineObjects::Camera^ newCamera = gcnew Engine::EngineObjects::Camera3D("Camera",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							parent
						));

					scene->AddObjectToScene(newCamera);
				}
				if (ImGui::MenuItem("Camera2D"))
				{
					Engine::EngineObjects::Camera^ newCamera = gcnew Engine::EngineObjects::Camera2D("Camera",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							parent
						));

					scene->AddObjectToScene(newCamera);
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("2D/3D"))
			{
				if (ImGui::BeginMenu("2D"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						auto sprite = gcnew Engine::EngineObjects::Sprite(
							"Sprite",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(1, 1, 1),
								parent
							)
						);

						scene->AddObjectToScene(sprite);
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("3D"))
				{
					if (ImGui::MenuItem("ModelRenderer"))
					{
						auto modelRenderer = gcnew Engine::EngineObjects::Geometry::ModelRenderer(
							"ModelRenderer",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 0,0,0 }),
								parent
							)
						);

						scene->AddObjectToScene(modelRenderer);
					}

					if (ImGui::MenuItem("MeshRenderer"))
					{
						auto meshRenderer = gcnew Engine::EngineObjects::Geometry::MeshRenderer(
							"MeshRenderer",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 1,1,1 }),
								parent
							)
						);

						scene->AddObjectToScene(meshRenderer);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Physics"))
			{
#ifdef USE_BULLET_PHYS
				if (ImGui::MenuItem("RigidBody"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::Physics::RigidBody(
						"RigidBody",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							parent
						)
					);

					scene->AddObjectToScene(meshRenderer);
				}
				/*
				if (ImGui::MenuItem("Collider"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::Physics::Collider(
						"Collider",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							scene->GetDatamodelMember("workspace")->transform
						)
					);

					scene->AddObjectToScene(meshRenderer);
				}
				*/

				if (ImGui::BeginMenu("Colliders"))
				{
					if (ImGui::MenuItem("Box Collider"))
					{
						auto meshRenderer = gcnew Engine::EngineObjects::Physics::BoxCollider(
							"BoxCollider",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(1, 1, 1),
								parent
							)
						);

						scene->AddObjectToScene(meshRenderer);
					}

					if (ImGui::MenuItem("Capsule Collider"))
					{
						auto meshRenderer = gcnew Engine::EngineObjects::Physics::CapsuleCollider(
							"CapsuleCollider",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(1, 1, 1),
								parent
							)
						);

						scene->AddObjectToScene(meshRenderer);
					}

					if (ImGui::MenuItem("Mesh Collider"))
					{
						auto meshRenderer = gcnew Engine::EngineObjects::Physics::MeshCollider(
							"MeshCollider",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(0, 0, 0),
								Engine::Components::Vector3(1, 1, 1),
								parent
							)
						);

						scene->AddObjectToScene(meshRenderer);
					}

					ImGui::EndMenu();
				}

#else
				ImGui::Text("Engine not compiled with physics engine module");
#endif
				ImGui::EndMenu();
			}

			ImGui::Separator();


			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("AudioSource"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::AudioSource(
						"AudioSource",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							nullptr
						)
					);

					scene->AddObjectToScene(meshRenderer);
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Lighting"))
			{
#ifdef USE_ILLUMINA
				if (ImGui::MenuItem("Point Light"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::LightSource(
						"Point Light",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_POINT,
						Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
						1.0f,
						1
					);
					meshRenderer->SetParent(lightManager);

					scene->AddObjectToScene(meshRenderer);
				}

				if (ImGui::MenuItem("Directional Light"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::LightSource(
						"Directional Light",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_DIRECTIONAL,
						Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
						1.0f,
						1
					);
					meshRenderer->SetParent(lightManager);
					scene->AddObjectToScene(meshRenderer);
				}

				if (ImGui::MenuItem("Spot Light"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::LightSource(
						"Spot Light",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(0, 0, 0),
							Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_SPOT,
						Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
						1.0f,
						1
					);
					meshRenderer->SetParent(lightManager);

					scene->AddObjectToScene(meshRenderer);
				}
#else
				ImGui::Text("Engine not compiled with illumina lighting module");
#endif

				ImGui::EndMenu();
			}


			ImGui::Separator();

			if (ImGui::BeginMenu("UI"))
			{
				// Text, Labels, Images.
				if (ImGui::BeginMenu("Static"))
				{
					if (ImGui::MenuItem("Image"))
					{
						auto image = gcnew Engine::EngineObjects::UI::Image("Image", gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(),
							Engine::Components::Vector3(),
							Engine::Components::Vector3(1, 1, 1),
							scene->GetDatamodelMember("gui")->getTransform()
						));

						scene->AddObjectToScene(image);
					}

					ImGui::EndMenu();
				}

				// Viewports, Renderers
				if (ImGui::BeginMenu("Dynamic"))
				{
					if (ImGui::MenuItem("RenderSurface3D"))
					{
						auto image = gcnew Engine::EngineObjects::Surface::RenderSurface3D("RenderSurface3D", gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(),
							Engine::Components::Vector3(),
							Engine::Components::Vector3(1, 1, 1),
							parent
						));

						scene->AddObjectToScene(image);
					}

					ImGui::EndMenu();
				}

				// Buttons, Text Fields
				if (ImGui::BeginMenu("Interactables"))
				{
					if (ImGui::MenuItem("Button"))
					{
						auto button = gcnew Engine::EngineObjects::UI::Button("Button", gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3(),
							Engine::Components::Vector3(),
							Engine::Components::Vector3(1, 1, 1),
							scene->GetDatamodelMember("gui")->getTransform()
						));

						scene->AddObjectToScene(button);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("User Scripts"))
			{
				if (ImGui::MenuItem("Lua Script"))
				{
					Engine::EngineObjects::LuaScript^ luaScript = gcnew Engine::EngineObjects::LuaScript("LuaScript",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							parent
						));

					scene->AddObjectToScene(luaScript);
				}

				ImGui::Separator();

				for each (auto assembly in assemblies)
				{
					if (assembly == nullptr) continue;
					if (assembly->getLoadedAssembly() == nullptr) continue;

					if (!assembly->getLoadedAssembly()->Equals(System::Reflection::Assembly::GetExecutingAssembly()))
					{
						for each (auto T in assembly->GetAssemblyTypes())
						{
							if (!T->Namespace->Equals(""))
							{
								if (ImGui::BeginMenu(CastToNative(T->Namespace)))
								{
									if (ImGui::MenuItem(CastToNative(T->Name)))
									{
										Engine::EngineObjects::ScriptBehaviour^ retn = assembly->Create<Engine::EngineObjects::ScriptBehaviour^>(T->FullName);

										scene->AddObjectToScene(retn);
									}

									ImGui::EndMenu();
								}
							}
							else
							{
								if (ImGui::MenuItem(CastToNative(T->Name)))
								{
									Engine::EngineObjects::ScriptBehaviour^ retn = assembly->Create<Engine::EngineObjects::ScriptBehaviour^>(T->FullName);

									scene->AddObjectToScene(retn);
								}
							}
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("Grid Renderer"))
				{
					Engine::EngineObjects::GridRenderer^ cubeRenderer = gcnew Engine::EngineObjects::GridRenderer("GridRenderer",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							scene->GetDatamodelMember("editor only")->getTransform()
						), 64, 1.0f);

					scene->AddObjectToScene(cubeRenderer);

				}

				if (ImGui::MenuItem("Editor Camera"))
				{
					auto camera3D = gcnew Engine::EngineObjects::Editor::EditorCamera("EditorCamera",
						gcnew Engine::Internal::Components::Transform(
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 0,0,0 }),
							Engine::Components::Vector3::create({ 1,1,1 }),
							nullptr
						)
					);

					scene->AddObjectToScene(camera3D);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Scripts", true))
		{
			ImGui::SeparatorText("Assemblies");
			if (ImGui::MenuItem("Hot Reload"))
			{

			}
			ImGui::SeparatorText("Tools");
			if (ImGui::MenuItem("Script Editor"))
			{
				codeEditor->setCodeEditorOpen(!codeEditor->isCodeEditorOpen());
			}

			if (ImGui::MenuItem("Generate Lua Bindings"))
			{
				Engine::Lua::VM::LuaVM::GenerateLuaBindings();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Editor", true))
		{
			if (ImGui::BeginMenu("Style", true))
			{
				if (ImGui::MenuItem("Style Editor"))
				{
					styleEditor = !styleEditor;
				}
				if (ImGui::MenuItem("Save/Load Style"))
				{
					b7 = true;
				}

				ImGui::EndMenu();
			}

			ImGui::Checkbox("FPS", &fpsCap);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help", true))
		{

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}
void EditorWindow::DrawHierarchy()
{
	if (hierarchyVisible && ImGui::Begin("Hierarchy", &hierarchyVisible))
	{
		ImGui::Text("Scene Objects: %d", scene->GetRenderQueue()->Count);
		ImGui::Separator();

		for (int x = 0; x < scene->GetRenderQueue()->Count; x++)
		{
			Engine::Internal::Components::GameObject^ obj = scene->GetRenderQueue()[x];

			auto reference = obj;
			auto type = obj->GetObjectType();


			if (reference != nullptr)
			{
				if (reference->getTransform()->parent != nullptr)
					continue;

				String^ refName = reference->name;
				refName += (reference->active == true) ? "" : " (INACTIVE)";

				if (reference->GetChildren()->Count > 0)
				{
					bool isOpen = ImGui::TreeNodeEx(CastStringToNative("##_" + refName + "_(ENGINE_PROTECTED)_" + "###_" + reference->getTransform()->GetUID() + x).c_str());
					ImGui::SameLine();

					if (type == ObjectType::Datamodel || type == ObjectType::LightManager || reference->isProtected())
					{
						if (ImGui::Selectable(CastToNative(refName + " (ENGINE PROTECTED)" + "###" + reference->getTransform()->GetUID() + x)))
						{
							if (reparentLock)
								reparentObject = reference;
							else if (selectionLock)
								selectionObject = reference;
							else
							{
								readonlyLock = true;
								selectedObject = reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}
					else if (reference->getTransform()->parent == nullptr)
					{
						if (ImGui::Selectable(CastToNative(refName + " (UNPARENTED)" + "###" + reference->getTransform()->GetUID() + x)))
						{
							if (reparentLock)
								reparentObject = reference;
							else if (selectionLock)
								selectionObject = reference;
							else
							{
								readonlyLock = false;
								selectedObject = reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}

					if (isOpen)
					{
						DrawHierarchyInherits(scene, reference, 1);
						ImGui::TreePop();
					}
				}
				else
				{

					if (type == ObjectType::Datamodel || type == ObjectType::LightManager || reference->isProtected())
					{
						if (ImGui::Selectable(CastToNative(refName + " (ENGINE PROTECTED)" + "###" + reference->getTransform()->GetUID() + x)))
						{
							if (reparentLock)
								reparentObject = reference;
							else if (selectionLock)
								selectionObject = reference;
							else
							{
								readonlyLock = true;
								selectedObject = reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}
					else if (reference->getTransform()->parent == nullptr)
					{
						if (ImGui::Selectable(CastToNative(refName + " (UNPARENTED)" + "###" + reference->getTransform()->GetUID() + x)))
						{
							if (reparentLock)
								reparentObject = reference;
							else if (selectionLock)
								selectionObject = reference;
							else
							{
								readonlyLock = false;
								selectedObject = reference;
								selectedObjectIndex = scene->GetRenderQueue()->IndexOf(selectedObject);
							}
						}
					}
				}
			}

		}
	}

	if (hierarchyVisible)
		ImGui::End();
}
void EditorWindow::DrawProperties()
{
	if (propertiesVisible && ImGui::Begin("Properties", &propertiesVisible, ImGuiWindowFlags_MenuBar))
	{
		if (selectedObject == nullptr)
		{
			ImGui::Text("Select an object to edit it's properties.");
		}
		else
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Instance Properties"))
				{
					if (ImGui::BeginMenu("Active"))
					{
						if (ImGui::MenuItem("Set Active"))
						{
							selectedObject->SetActive(true);
						}

						if (ImGui::MenuItem("Set Inactive"))
						{
							selectedObject->SetActive(false);
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Rendering"))
					{
						{
							switch (selectedObject->viewSpace)
							{
							case ViewSpace::VNone:
								tmp1 = 0;
								break;
							case ViewSpace::V2D:
								tmp1 = 1;
								break;
							case ViewSpace::V3D:
								tmp1 = 2;
								break;
							}

							const char* types[]{ "None", "2D", "3D" };
							if (ImGui::Combo("###CURR_MODE", &tmp1, types, IM_ARRAYSIZE(types)))
							{
								switch (tmp1)
								{
								case 0:
									selectedObject->viewSpace = ViewSpace::VNone;
									break;
								case 1:
									selectedObject->viewSpace = ViewSpace::V2D;
									break;
								case 2:
									selectedObject->viewSpace = ViewSpace::V3D;
									break;
								}
							}
						}

						{
							std::string layer = "";

							if (selectedObject->layerMask != nullptr)
								layer = CastStringToNative(selectedObject->layerMask->layerName);
							else
								layer = "Select Layer";

							Layer^ l = nullptr;

							std::vector<std::string> layers = LayerManager::getLayerNames();

							if (ImGui::BeginCombo("###CURR_LAYER", layer.c_str(), ImGuiComboFlags_None))
							{
								for (std::string tmp : layers)
								{
									bool isSelected = false;

									String^ managedType = gcnew String(layer.data());
									String^ data = gcnew String(tmp.data());

									data = data->Substring(data->IndexOf("-") + 2);

									if (data->Equals(managedType))
									{
										isSelected = true;
									}

									if (ImGui::Selectable(tmp.c_str(), &isSelected))
									{
										data = gcnew String(tmp.c_str());

										String^ buffer = data->Substring(0, data->IndexOf(" -"));

										l = LayerManager::GetLayerFromId(int::Parse(buffer));

										if (l == nullptr)
										{
											printError("Failed getting layer (is null)");
											return;
										}

										printConsole("Swap layer from " + selectedObject->layerMask->layerName + " to " + l->layerName);

										if (l != nullptr)
											selectedObject->SetLayerMask(l);
									}

									if (isSelected)
										ImGui::SetItemDefaultFocus();
								}

								ImGui::EndCombo();
							}
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Tagging"))
					{
						std::string objectName = CastStringToNative(selectedObject->GetTag());

						if (ImGui::InputText("Tag", &objectName))
						{
							selectedObject->SetTag(gcnew String(objectName.c_str()));
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Instance Actions"))
				{
					if (ImGui::MenuItem("Change Parent"))
					{
						if (!readonlyLock)
							reparentLock = true;
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Destroy Object"))
					{
						ObjectManager::singleton()->Destroy(selectedObject);
						selectedObject = nullptr;
						return;
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Export Prefab"))
					{
						jsonData = Serialize(gcnew Prefab(selectedObject));
						OpenFileExplorer("Save Prefab", Engine::Editor::Gui::explorerMode::Save, gcnew Engine::Editor::Gui::onFileSelected(&SaveToFile));
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Class"))
				{
					if (ImGui::MenuItem("Protect Member"))
					{
						selectedObject->protectMember();
					}

					if (ImGui::MenuItem("Unprotect Member"))
					{
						if (selectedObject->GetObjectType() == ObjectType::Datamodel || selectedObject->GetObjectType() == ObjectType::LightManager)
							return;

						selectedObject->unprotectMember();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}


			ImGui::SeparatorText("Object Properties");

			std::string objectName = CastStringToNative(selectedObject->name);


			if (ImGui::InputText("Name", &objectName, ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
			{
				selectedObject->name = gcnew String(objectName.c_str());
			}

			ImGui::SeparatorText("Transform");

			{
				const char* opts[2] = { "World", "Local" };

				ImGui::Combo("###POSITION_SELECTOR", &positionSelector, opts, IM_ARRAYSIZE(opts));
				if (positionSelector == 0)
				{
					// position
					float pos[3] = {
						selectedObject->getTransform()->position.x,
						selectedObject->getTransform()->position.y,
						selectedObject->getTransform()->position.z
					};

					if (ImGui::DragFloat3("Position", pos, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
					{
						selectedObject->getTransform()->position = Engine::Components::Vector3(pos[0], pos[1], pos[2]);
					}
				}
				else
				{
					// local position
					float pos[3] = {
						selectedObject->getTransform()->localPosition.x,
						selectedObject->getTransform()->localPosition.y,
						selectedObject->getTransform()->localPosition.z
					};

					if (ImGui::DragFloat3("Local Position", pos, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
					{
						selectedObject->getTransform()->localPosition = Engine::Components::Vector3(pos[0], pos[1], pos[2]);
					}
				}

				// rotation
				float rot[3] = {
					selectedObject->getTransform()->rotation.x,
					selectedObject->getTransform()->rotation.y,
					selectedObject->getTransform()->rotation.z
				};

				if (ImGui::DragFloat3("Rotation", rot, 5.0f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
				{
					selectedObject->getTransform()->rotation = Engine::Components::Vector3(rot[0], rot[1], rot[2]);
				}

				// scale

				float scale[3] = {
					selectedObject->getTransform()->scale.x,
					selectedObject->getTransform()->scale.y,
					selectedObject->getTransform()->scale.z
				};

				if (ImGui::DragFloat3("Scale", scale, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
				{
					selectedObject->getTransform()->scale = Engine::Components::Vector3(scale[0], scale[1], scale[2]);
				}
			} // Transform

			SpecializedPropertyEditor(selectedObject);
		}
	}

	if (propertiesVisible)
		ImGui::End();
}
void EditorWindow::DrawAssets()
{
	if (assetsVisible && ImGui::Begin("Assets", &assetsVisible, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::MenuItem("Pack Setup"))
				{
					packDataFileName = "";
					loadedAssets = gcnew System::Collections::Generic::List<String^>();
					b5 = true;
				}
				if (ImGui::MenuItem("Scene Loader Setup"))
				{
					codeEditor->SetEditorCode(CastStringToNative("Data/" + scene->sceneRequirements + ".asset"), CastStringToNative(System::IO::File::ReadAllText("Data/" + scene->sceneRequirements + ".asset")));
					codeEditor->setCodeEditorOpen(true);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImVec2 size = ImGui::GetWindowSize();

		const char* constData[] = { "ALL", "MODELS", "TEXTURES", "SOUND", "MUSIC", "SCRIPTS", "PREFABS", "MATERIALS" };
		ImGui::Text("Asset Name: ");
		ImGui::SameLine();
		ImGui::InputText("###AssetNameFilter", &asset_filter_name);
		ImGui::Text("Display assets: ");
		ImGui::SameLine();
		if (ImGui::Combo("###Display assets: ", &displayingAsset, constData, IM_ARRAYSIZE(constData)))
		{
			switch (displayingAsset)
			{
			case 0:
				displayingAssets = assetDisplay::ALL;
				break;
			case 1:
				displayingAssets = assetDisplay::MODELS;
				break;
			case 2:
				displayingAssets = assetDisplay::TEXTURES;
				break;
			case 3:
				displayingAssets = assetDisplay::SOUND;
				break;
			case 4:
				displayingAssets = assetDisplay::MUSIC;
				break;
			case 5:
				displayingAssets = assetDisplay::SCRIPTS;
				break;
			case 6:
				displayingAssets = assetDisplay::PREFAB;
				break;
			case 7:
				displayingAssets = assetDisplay::MATERIALS;
				break;
			}
		}

		ImGui::Separator();

		if (ImGui::BeginListBox("###ASSETS", { size.x - 20, size.y - 82 }))
		{
			createAssetEntries("./Data/");

			ImGui::EndListBox();
		}
	}

	if (assetsVisible)
		ImGui::End();
}
void EditorWindow::DrawImGuizmo()
{
	if (selectedObject == nullptr)
		return;

	Engine::EngineObjects::Camera^ camera = ObjectManager::singleton()->GetMainCamera(false);

	if (camera == nullptr)
		return;

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGuizmo::BeginFrameViewport(windowPos, windowSize);

	ImGuizmo::SetOrthographic(camera->is3DCamera() == false);
	ImGuizmo::SetDrawlist();

	float matrix[16];
	float pos[3] = { selectedObject->transform->position.x, selectedObject->transform->position.y, selectedObject->transform->position.z };
	float rot[3] = { selectedObject->transform->rotation.x, selectedObject->transform->rotation.y, selectedObject->transform->rotation.z };
	float sca[3] = { selectedObject->transform->scale.x, selectedObject->transform->scale.y, selectedObject->transform->scale.z };

	float view[16];
	float projection[16];

	float aspect = Engine::Scripting::Screen::Width / Engine::Scripting::Screen::Height;
	float nearPlane = camera->nearPlane;
	float farPlane = camera->farPlane;

	/*
	
	float camPos[3] = { camera->transform->position.x, camera->transform->position.y, camera->transform->position.z };
	float camRot[3] = { camera->transform->rotation->x, camera->transform->rotation->y, camera->transform->rotation->z };
	float camSca[3] = { camera->transform->scale->x, camera->transform->scale->y, camera->transform->scale->z };

	RAYLIB::Matrix rotX = RAYMATH::MatrixRotateX(camRot[0]);
	RAYLIB::Matrix rotY = RAYMATH::MatrixRotateY(camRot[1]);
	RAYLIB::Matrix rotZ = RAYMATH::MatrixRotateZ(camRot[2]);

	RAYLIB::Matrix rotation = RAYMATH::MatrixMultiply(rotY, RAYMATH::MatrixMultiply(rotX, rotZ));

	RAYLIB::Matrix translation = RAYMATH::MatrixTranslate(-camPos[0], -camPos[1], -camPos[2]);

	// View matrix = rotation * translation
	RAYLIB::Matrix viewMtx = RAYMATH::MatrixMultiply(rotation, translation);
	*/

	RAYLIB::Matrix projMtx;
	RAYLIB::Matrix viewMtx = RAYLIB::GetCameraMatrix(*((RAYLIB::Camera*)camera->get()));

	if (camera->is3DCamera())
	{
		float fovY = camera->fov * DEG2RAD;
		float top = tanf(fovY / 2.0f) * nearPlane;
		float right = top * aspect;

		projMtx = RAYMATH::MatrixPerspective(fovY, aspect, nearPlane, farPlane);
	}
	else
	{
		float width = camera->fov;
		float height = width / aspect;

		float left = -width / 2;
		float right = width / 2;
		float bottom = -height / 2;
		float top = height / 2;

		projMtx = RAYMATH::MatrixOrtho(left, right, bottom, top, nearPlane, farPlane);
	}

	ImGuizmo::RecomposeMatrixFromComponents(pos, rot, sca, matrix);
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

	CopyRaylibMatrixToFloat16(viewMtx, view);
	CopyRaylibMatrixToFloat16(projMtx, projection);

	ImGuizmo::Manipulate(view, projection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix);

	if (ImGuizmo::IsUsing())
	{
		ImGuizmo::DecomposeMatrixToComponents(matrix, pos, rot, sca);

		selectedObject->transform->position = Engine::Components::Vector3(pos[0], pos[1], pos[2]);
		selectedObject->transform->rotation = Engine::Components::Vector3(rot[0], rot[1], rot[2]);
		selectedObject->transform->scale = Engine::Components::Vector3(sca[0], sca[1], sca[2]);
	}
}
void EditorWindow::DrawToolbar()
{
	ImGui::Begin("ToolBar", nullptr, ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize );
	{
		int width = (int)Engine::Scripting::Screen::Width;
		ImVec2 size = ImVec2(
			width,
			32+8
		);
		ImGui::SetWindowSize(size);

		if (rlImGuiImageButtonSize("###SELECTION", &dragTexture, { 24, 24 }))
		{
			mCurrentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;
		}
		ImGui::SameLine();
		if (rlImGuiImageButtonSize("###TRANSLATE", &translateTexture, { 24, 24 }))
		{
			mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();
		if (rlImGuiImageButtonSize("###ROTATE", &rotateTexture, { 24, 24 }))
		{
			mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();
		if (rlImGuiImageButtonSize("###SCALE", &scaleTexture, { 24, 24 }))
		{
			mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		ImGui::SameLine();
		ImGui::SameLine();
		ImGui::SameLine();
		ImGui::SameLine();
		if (rlImGuiImageButtonSize("###PLAY", &playTexture, { 24, 24 }))
		{
			TogglePlayMode(this);
		}
		ImGui::SameLine();
		if (rlImGuiImageButtonSize("###STOP", &stopTexture, { 24, 24 }))
		{
			TogglePlayMode(this);
		}
	}
	ImGui::End();
}
void EditorWindow::DrawImGui()
{
	auto viewPort = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(viewPort->ID, viewPort, ImGuiDockNodeFlags_None);

	DrawMainMenuBar();
	DrawToolbar();
	DrawHierarchy();

	if (codeEditor->isCodeEditorOpen())
		codeEditor->Draw();

	if (materialEditor->isMaterialEditorOpen())
		materialEditor->GUI();

	DrawAssets();

	if (ifd::FileDialog::Instance().IsDone("TestFileDialog"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			std::string res = ifd::FileDialog::Instance().GetResult().u8string();
			printf("DIRECTORY[%s]\n", res.c_str());
		}

		ifd::FileDialog::Instance().Close();
	}

	if (styleEditor)
	{
		ImGui::Begin("StyleEditor");
		{
			ImGui::ShowStyleEditor(&ImGui::GetStyle());
			ImGui::End();
		}
	}

	if (scenevpVisible && ImGui::Begin("Scene Viewport", &scenevpVisible))
	{
		ImGuiStyle style = ImGui::GetStyle();

		ImVec2 oldPadding = style.WindowPadding;
		ImVec2 oldFramePadding = style.FramePadding;

		style.WindowPadding = ImVec2(0, 0);
		style.FramePadding = ImVec2(0, 0);

		ScopedStyle scopedStyle = ScopedStyle();
		scopedStyle.Set(style);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		Screen::setX(windowPos.x);
		Screen::setY(windowPos.y);
		Screen::setWidth(windowSize.x);
		Screen::setHeight(windowSize.y);

		rlImGuiImageRenderTextureCustom(&renderPipeline->framebufferTexturePtr->getInstance(), new int[2] { (int)ImGui::GetWindowSize().x, (int)ImGui::GetWindowSize().y }, new float[2] {17.5f, 35.0f});
		DrawImGuizmo();

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

			ImGui::RenderNotifications(ImGui::GetWindowPos(), ImGui::GetWindowSize());

			ImGui::PopStyleVar(2);
		}

		if (ImGui::IsWindowFocused())
		{
			if (InputManager::IsKeyPressed(KeyCodes::KEY_ONE))
			{
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}

			if (InputManager::IsKeyPressed(KeyCodes::KEY_TWO))
			{
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			}

			if (InputManager::IsKeyPressed(KeyCodes::KEY_THREE))
			{
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			}
		}

		scopedStyle.Reset();
	}
	
	
	if (scenevpVisible)
		ImGui::End();

	DrawConsole();

	DrawProperties();

	if (b1)
	{
		ImGui::OpenPopup("New Scene");
	}
	else if (b2)
	{
		ImGui::OpenPopup("Create New Scene");
	}
	else if (b3)
	{
		ImGui::OpenPopup("Open Scene");
	}
	else if (b4)
	{
		ImGui::OpenPopup("AssetPack Editor");
	}
	else if (b5)
	{
		ImGui::OpenPopup("Pack Setup");
	}
	else if (b6)
	{
		//ImGui::OpenPopup("Scene Loader Editor");
	}
	else if (b7)
	{
		ImGui::OpenPopup("Save/Load Style");
	}
	else if (b8)
	{
		ImGui::OpenPopup("Layer Editor");
	}
	else if (b9)
	{
		ImGui::OpenPopup("Engine Configuration");
	}

	if (visualizeError)
	{
		ImGui::OpenPopup("Unexpected Error");
	}

	fileExplorer->DrawExplorer();
	fileExplorer->TryComplete();

	if (reparentLock)
	{
		if (reparentObject != nullptr)
		{
			selectedObject->SetParent(reparentObject);
			reparentLock = false;
		}
	}
	else
	{
		reparentObject = nullptr;
	}

	// popups

	if (ImGui::BeginPopupModal("Pack Setup", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
	{
		for (int x = 0; x < loadedAssets->Count; x++)
		{
			std::string buffer = CastStringToNative(loadedAssets[x]);

			std::string name = "###" + std::string(CastToNative(loadedAssets[x])) + "###_index" + std::to_string(x);
			if (ImGui::InputText(name.c_str(), &buffer))
			{
				loadedAssets[x] = gcnew String(buffer.c_str());
			}
		}

		ImGui::Separator();
		{
			if (ImGui::Button("+"))
			{
				loadedAssets->Add("");
			}
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				if (loadedAssets->Count >= 1)
				{
					loadedAssets->RemoveAt(loadedAssets->Count - 1);
				}
			}
		}
		ImGui::Separator();


		if (ImGui::Button("Pack all the files"))
		{
			OpenFileExplorer("Select output file", Engine::Editor::Gui::explorerMode::Save, gcnew Engine::Editor::Gui::onFileSelected(this, &EditorWindow::PackData));

			b5 = false;

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			b5 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Engine Configuration", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
	{
		auto config = Engine::Config::EngineConfiguration::singleton();

		{
			std::string buff = CastStringToNative(config->windowName);

			ImGui::Text("Window Name: ");
			ImGui::SameLine();
			if (ImGui::InputText("##WINDOW_NAME", &buff))
			{
				Engine::Config::EngineConfiguration::singleton()->windowName = gcnew String(buff.c_str());
			}
		}

		ImGui::SeparatorText("Resolution");

		{
			ImGui::Text("Window X:");
			ImGui::SameLine();
			ImGuiNET::ImGui::InputInt("###_ENGINECONFIG_WINDOW_X", config->resolution->x);

			ImGui::Text("Window Y:");
			ImGui::SameLine();
			ImGuiNET::ImGui::InputInt("###_ENGINECONFIG_WINDOW_Y", config->resolution->y);

			if (ImGui::Button("Set Screen Center"))
			{
				config->resolution->x = -1;
				config->resolution->y = -1;
			}

			ImGui::Text("Window Width:");
			ImGui::SameLine();
			ImGuiNET::ImGui::InputInt("###_ENGINECONFIG_WINDOW_WIDTH", config->resolution->w);

			ImGui::Text("Window Height:");
			ImGui::SameLine();
			ImGuiNET::ImGui::InputInt("###_ENGINECONFIG_WINDOW_HEIGHT", config->resolution->h);

			if (ImGui::Button("Set Window Mode Fullscreen"))
			{
				config->resolution->w = -1;
				config->resolution->h = -1;
			}
		}

		ImGui::SeparatorText("FrameRate");

		{
			ImGui::Text("Target FPS:");
			ImGuiNET::ImGui::SliderInt("###_ENGINECONFIG_TARGETFPS", config->targetFPS, 1, 1000);
		}

		ImGui::SeparatorText("Window Flags");

		{
			if (ImGui::BeginListBox("###_ENGINECONFIG_WINDOW_FLAGS"))
			{
				{
					ImGui::Text("VSync:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_VSync"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->VSync);
				}

				{
					ImGui::Text("FullScreen:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_FullScreen"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->Fullscreen);
				}

				{
					ImGui::Text("Resizable:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_Resizable"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->Resizable);
				}

				{
					ImGui::Text("Undecorated:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_Undecorated"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->Undecorated);
				}

				{
					ImGui::Text("Hidden:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_Hidden"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->Hidden);
				}

				{
					ImGui::Text("Transparent:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_Transparent"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->Transparent);
				}

				{
					ImGui::Text("Borderless Windowed:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_BorderlessFullScreen"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->BorderlessWindowed);
				}

				{
					ImGui::Text("Run On Minimized:");
					ImGui::SameLine();
					ImGuiNET::ImGui::Checkbox(gcnew String("###ENGINECONFIG_RunOnMinimized"), Engine::Config::EngineConfiguration::singleton()->_windowFlags->AlwaysRun);
				}

				ImGui::EndListBox();
			}
		}

		if (ImGui::Button("Close"))
		{
			b9 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Layer Editor", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
	{
		if (ImGui::BeginListBox("###LAYER_LIST"))
		{
			auto% layers = LayerManager::GetLayers();

			for (int x = 0; x < layers->Count; x++)
			{
				Layer^ layer = layers[x];


			}



			ImGui::EndListBox();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Save/Load Style", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
	{
		std::string tmp = styleFN;

		ImGui::Text("File Name:");
		ImGui::SameLine();
		if (ImGui::InputText("###FILE_NAME", &tmp, ImGuiInputTextFlags_CallbackCompletion))
		{
			styleFN = tmp;
		}

		ImGui::NewLine();

		if (ImGui::Button("Save"))
		{
			ImGui::SaveStyle(styleFN.c_str(), ImGui::GetStyle());
			b7 = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			ImGui::LoadStyle(styleFN.c_str(), ImGui::GetStyle());
			b7 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Scene Loader Editor", (bool*)false, ImGuiWindowFlags_NoScrollbar))
	{
		int heapAlloc = 512;
		char* data = new char[8192];
		auto t = System::IO::File::ReadAllText("Data/" + scene->sceneRequirements + ".asset");

		int size = t->ToCharArray()->Length + heapAlloc;
		data = new char[size];

		strcpy(data, CastStringToNative(t).c_str());

		if (ImGui::InputTextMultiline("", data, size, { ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 60 }))
		{
			System::String^ str = gcnew System::String(data);

			System::IO::File::WriteAllText("Data/" + scene->sceneRequirements + ".asset", str);
		}

		if (ImGui::Button("Close"))
		{
			b6 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("New Scene", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Do you want to create a new scene?\nAll the unsaved changes will be discarded.");
		ImGui::Separator();
		if (ImGui::Button("New"))
		{
			b1 = false;
			b2 = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			b1 = false;
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Create New Scene", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Input Scene Name: ");
		ImGui::SameLine();
		ImGui::InputText("", &fileName);

		if (ImGui::Button("Create Scene"))
		{
			scene = SceneManager::CreateScene(gcnew System::String(fileName.c_str()));
			scene->LoadScene();
			create();
			ImGui::CloseCurrentPopup();
			b2 = false;
		}
		if (ImGui::Button("Cancel"))
		{
			b2 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Open Scene", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Input Scene Name: ");
		ImGui::SameLine();
		ImGui::InputText("", &fileName);

		if (ImGui::Button("Open Scene"))
		{
			SceneManager::UnloadScene(scene);
			SceneManager::LoadSceneFromFile(gcnew System::String(fileName.c_str()), passwd, scene);
			//scene->LoadScene();

			packedData = scene->getSceneDataPack();

			create();
			ImGui::CloseCurrentPopup();
			b3 = false;
		}
		if (ImGui::Button("Cancel"))
		{
			b3 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("AssetPack Editor", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize))
	{
		for (int x = 0; x < scene->assetPacks->Count; x++)
		{
			std::string data = CastStringToNative(scene->assetPacks[x]);
			std::string name = "###" + std::string(CastToNative(scene->assetPacks[x]));
			if (ImGui::InputText(name.c_str(), &data, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				scene->assetPacks[x] = gcnew String(data.c_str());
			}
		}

		ImGui::Separator();
		{
			if (ImGui::Button("+"))
			{
				scene->assetPacks->Add("");
			}
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				if (scene->assetPacks->Count >= 1)
				{
					scene->assetPacks->RemoveAt(scene->assetPacks->Count - 1);
				}
			}
		}
		ImGui::Separator();

		if (ImGui::Button("Close"))
		{
			b4 = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	RenderFBXConverter();

	ShowError();
}
void EditorWindow::Exit()
{
	UnloadTexture(modelTexture);
	UnloadTexture(materialTexture);
	UnloadTexture(soundTexture);
	UnloadTexture(scriptTexture);
	Engine::Utils::LogReporter::singleton->CloseThread();
	dataPack.FreeAll();
	exit(0);
}
void EditorWindow::RegisterKeybinds()
{
	if (InputManager::IsKeyDown(KeyCodes::KEY_LEFT_CONTROL) && InputManager::IsKeyPressed(KeyCodes::KEY_P))
	{
		TogglePlayMode(this);
	}

	if (EngineState::PlayMode)
	{
	}
	else
	{
		if (selectedObject != nullptr && InputManager::IsKeyDown(KeyCodes::KEY_DELETE))
		{
			Singleton<ObjectManager^>::Instance->Destroy(selectedObject);
			selectedObject = nullptr;
		}

		if (InputManager::IsKeyDown(KeyCodes::KEY_LEFT_CONTROL) && InputManager::IsKeyPressed(KeyCodes::KEY_S))
		{
			print("[GoldEngine]:", "Saving Scene");
			SceneManager::SaveSceneToFile(scene, passwd);
		}

		if (InputManager::IsKeyPressed(KeyCodes::KEY_LEFT_ALT))
		{
			showCursor = !showCursor;

			if (!showCursor)
			{
				EnableCursor();
			}
			else
			{
				DisableCursor();
			}
		}

		if (InputManager::IsKeyPressed(KeyCodes::KEY_ESCAPE))
		{
			selectedObject = nullptr;
		}
	}

}
void EditorWindow::Draw()
{
	if (!scene->sceneLoaded())
		return;

	if (EngineState::PlayMode)
	{
		renderPipeline->ExecuteRenderWorkflow(this, scene);
	}
	else
	{
		renderPipeline->ExecuteRenderWorkflow_Editor(this, scene);
	}
}
void EditorWindow::create()
{
	Engine::EngineObjects::Private::Scene^ gameRoot = nullptr;

	if (!scene->ExistsMember("game"))
	{
		gameRoot = gcnew Engine::EngineObjects::Private::Scene(
			"game",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			)
		);
		scene->PushToRenderQueue(gameRoot);
	}
	else
	{
		try
		{
			gameRoot = scene->GetMember("game")->ToObjectType<Engine::EngineObjects::Private::Scene^>();
		}
		catch (Exception^ ex)
		{
			gameRoot = gcnew Engine::EngineObjects::Private::Scene(
				"game",
				gcnew Engine::Internal::Components::Transform(
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					nullptr
				)
			);
		}
	}

	auto workspace = scene->GetDatamodelMember("workspace", true);
	workspace->setParent(gameRoot);
	auto editor_only = scene->GetDatamodelMember("editor only", true);
	editor_only->setParent(gameRoot);
	auto gui = scene->GetDatamodelMember("gui", true);
	gui->setParent(gameRoot);
	auto daemonParent = scene->GetDatamodelMember("daemons", true);
	daemonParent->setParent(gameRoot);

#ifdef USE_BULLET_PHYS

	if (!scene->ExistsMember("PhysicsService"))
	{
		auto physicsService = gcnew Engine::EngineObjects::Physics::PhysicsService("PhysicsService",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			)
		);
		physicsService->setParent(gameRoot);

		scene->PushToRenderQueue(physicsService);
	}

#endif

#ifdef USE_ILLUMINA
	if (!scene->ExistsMember("lighting"))
	{
		lightManager = gcnew LightManager("lighting",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			"Data/Engine/Shaders/Illumina/Illumina.vert",
			"Data/Engine/Shaders/Illumina/Illumina.frag"
		);
		lightManager->setParent(gameRoot);
		lightManager->protectMember();

		scene->PushToRenderQueue(lightManager);
	}
	else
	{
		try
		{
			lightManager = scene->GetMember("lighting")->ToObjectType<LightManager^>();
		}
		catch (Exception^ ex)
		{
			lightManager = lightManager = gcnew LightManager("lighting",
				gcnew Engine::Internal::Components::Transform(
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					nullptr
				),
				"Data/Engine/Shaders/Illumina/Illumina.vert",
				"Data/Engine/Shaders/Illumina/Illumina.frag"
			);
		}
		lightManager->setParent(gameRoot);
		lightManager->protectMember();
	}

	if (ObjectManager::singleton()->GetChildrenOf(daemonParent)->Count <= 0)
	{
		auto lightdm = gcnew Engine::EngineObjects::Daemons::LightDaemon("lightdm",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			lightManager
		);
		lightdm->SetParent(daemonParent);
		scene->PushToRenderQueue(lightdm);
	}
#endif

	auto editorCamera = ObjectManager::singleton()->GetFirstObjectOfType(Engine::EngineObjects::Editor::EditorCamera::typeid);

	if (editorCamera == nullptr)
	{
		auto camera3D = gcnew Engine::EngineObjects::Editor::EditorCamera("EditorCamera",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3::create({ 0,0,0 }),
				Engine::Components::Vector3::create({ 0,0,0 }),
				Engine::Components::Vector3::create({ 1,1,1 }),
				nullptr
			)
		);
		//camera3D->SetParent(((Engine::Internal::Components::GameObject^)Singleton<ObjectManager^>::Instance->GetDatamodel("workspace")));
		camera3D->setParent(gameRoot);
		scene->PushToRenderQueue(camera3D);
	}

	ImGuiToast toast(ImGuiToastType_Success, 5000);
	toast.setTitle("GoldEngine Editor");
	toast.setContent("Scene Loaded!");

	ImGui::InsertNotification(toast);
}
void EditorWindow::Init()
{
	modelTexture = LoadTexture("EditorAssets/Icons/Model.png");
	textureTexture = LoadTexture("EditorAssets/Icons/Texture.png");
	materialTexture = LoadTexture("EditorAssets/Icons/Material.png");
	soundTexture = LoadTexture("EditorAssets/Icons/Sound.png");
	scriptTexture = LoadTexture("EditorAssets/Icons/Script.png");
	translateTexture = LoadTexture("EditorAssets/Icons/Translate.png");
	rotateTexture = LoadTexture("EditorAssets/Icons/Rotate.png");
	scaleTexture = LoadTexture("EditorAssets/Icons/Scale.png");
	dragTexture = LoadTexture("EditorAssets/Icons/Mouse.png");
	playTexture = LoadTexture("EditorAssets/Icons/Run.png");
	stopTexture = LoadTexture("EditorAssets/Icons/Stop.png");

	cameraPosition = Engine::Components::Vector3(0, 0, 0);

	while (!scene->sceneLoaded())
		WaitTime(1.0);

	create();

	Logging::LogCustom("[GL Version]:", "Current OpenGL version is -> " + RLGL::rlGetVersion() + ".");

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	ImGui::GetIO().ConfigErrorRecovery = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableAssert = true;
	ImGui::GetIO().ConfigErrorRecoveryEnableDebugLog = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableTooltip = true;
}
void EditorWindow::Preload()
{
	dataPack.LoadDefaultAssets();
	ImGui::LoadStyle("EditorStyle.ini");

	if (FirstTimeBoot())
	{
		WinAPI::MBOXA(GetWindowHandle(), "LPVOID* voidFunc = (LPVOID*)nativeData;\nvoidFunc->Test();", "GoldEngine - Ver 0.5c - editor", 0x00000040L | 0x00000000L);
		Boot();
	}
	SetExitKey(KEY_NULL);

	renderPipeline = gcnew Engine::Render::Pipelines::LitPBR_SRP();

	SceneManager::LoadSceneFromFile(gcnew System::String(fileName.c_str()), passwd, scene);

	while (!scene->sceneLoaded())
	{
		RAYLIB::WaitTime(1.0f);
	}

	packedData = scene->getSceneDataPack();

	Init();
}
void EditorWindow::Update()
{
	if (!scene->sceneLoaded())
		return;

	Engine::EngineObjects::Camera^ camera = ObjectManager::singleton()->GetMainCamera();

	if (Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance != renderPipeline)
		renderPipeline = Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance;

	if (camera == nullptr)
		return;

	auto projectionMode = camera->getProjection();
	bool is3DCamera = (projectionMode == CamProjection::CAMERA_PERSPECTIVE);

	void* cameraLocal = camera->get();

	if (showCursor && camera->GetType() == Engine::EngineObjects::Editor::EditorCamera::typeid)
		UpdateCamera(((NativeCamera3D*)cameraLocal)->getCameraPtr(), CAMERA_FREE);

	if (fpsCap)
		SetFPS(60);
	else
		SetFPS(-1);

	auto renderQueue = scene->GetRenderQueue();

	msclr::lock^ lock = gcnew msclr::lock(renderQueue);
	if (lock->try_acquire(5000))
	{
		auto renderQue = renderQueue->ToArray();

		for each (GameObject ^ obj in renderQue)
		{
			if (scene->sceneLoaded())
			{
				obj->GameUpdate();
			}
		}

		renderQue->Clear(renderQue);
	}
	lock->release();

	engine_keybinds();
	RegisterKeybinds();

	Engine::GC::EngineGC::Update();
}

#pragma endregion

#else

#include "GameWindow.h"

#endif


/*
BOOTSTRAPS
1. C++ COM ITEROP
2. C++/CLI NATIVE
*/

extern "C"
{
	DllExport void InitializeGoldEngine()
	{
		passwd = Engine::Encryption::CypherLib::GetPasswordBytes(gcnew String(ENCRYPTION_PASSWORD));

#if PRODUCTION_BUILD
		gcnew GameWindow();
#else
		gcnew EditorWindow();
#endif
	}
}

public ref class Bootstrap
{
public:
	static void InitializeGoldEngine()
	{
		passwd = Engine::Encryption::CypherLib::GetPasswordBytes(gcnew String(ENCRYPTION_PASSWORD));

#if PRODUCTION_BUILD
		gcnew GameWindow();
#else
		gcnew EditorWindow();
#endif
	}
};


#ifdef _DEBUG
int main()
{
	InitializeGoldEngine();
}
#endif