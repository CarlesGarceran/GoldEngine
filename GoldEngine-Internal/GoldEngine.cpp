// ATTRIBUTES \\

#include "Instantiable.h"
#include "ExecuteInEditModeAttribute.h"

using namespace Engine::Attributes;

// DEPENDENCIES \\

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

// CUSTOM RENDERERS \\

#include "VoxelRenderer.h"

// INCLUDE ENGINE CLASSES \\

#include "SceneFormat.h"
#include "EngineIncludes.h"

#include "imguistyleserializer.h"

// Prefabs

#include "Object/Prefab.h"

// API

#include "Screen.h"
#include "Time.h"
#include "ImguiHook.h"
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

// audio

#include "Objects/Audio/AudioSource.h"

// physics

#ifdef USE_BULLET_PHYS

#include "Objects/Physics/CollisionType.h"
#include "Objects/Physics/Native/NativePhysicsService.h"
#include "Objects/Physics/RigidBody.h"
#include "Objects/Physics/PhysicsService.h"

#endif

// render pipelines

#include "Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "RenderPipelines/LitPBR_SRP.h"
#include "RenderPipelines/LightweightSRP.h"

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

static Newtonsoft::Json::JsonSerializerSettings^ SerializerSettings()
{
	auto serializerSettings = gcnew Newtonsoft::Json::JsonSerializerSettings();
	serializerSettings->Converters->Add(gcnew Newtonsoft::Json::Converters::StringEnumConverter());
	serializerSettings->TypeNameHandling = Newtonsoft::Json::TypeNameHandling::None;

	return serializerSettings;
}

static void engine_keybinds()
{
	if (InputManager::IsKeyPressed(KeyCodes::KEY_F11))
		ToggleFullscreen();
}

static void engine_bootstrap()
{
	Newtonsoft::Json::JsonConvert::DefaultSettings = gcnew Func<Newtonsoft::Json::JsonSerializerSettings^>(&SerializerSettings);

	if (!Directory::Exists("Bin/"))
		Directory::CreateDirectory("Bin");

	if (!Directory::Exists("Bin/Asm"))
		Directory::CreateDirectory("Bin/Asm");

	if (!Directory::Exists("Data"))
		Directory::CreateDirectory("Data");

	if (!Directory::Exists("Data/Keys/"))
		Directory::CreateDirectory("Data/Keys/");

	if (!File::Exists("Data/Keys/map.iv"))
	{
		FileStream^ fs = File::Open("Data/Keys/map.iv", System::IO::FileMode::OpenOrCreate);
		BinaryWriter^ writer = gcnew BinaryWriter(fs);

		Random^ r = gcnew Random();

		writer->Write(System::Guid::NewGuid().ToString());
		writer->Close();
		fs->Close();
	}

	if (!Directory::Exists("Data/UserData/"))
		Directory::CreateDirectory("Data/UserData/");
}

char fileName[] = "Level0";

#if !defined(PRODUCTION_BUILD)

#pragma region EDITOR ENGINE

// EDITOR UI

// extra tools (converters, etc...)

#include "AssimpImpl/AssimpConverter.h"
#include "EditorGUI.h"

Model mod;
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

std::string styleFN;
Texture modelTexture;
Texture materialTexture;
Texture scriptTexture;
Texture soundTexture;

bool fileDialogOpen = false;
int tmp1;
char* password = new char[512];
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

#include "EditorTools/CodeEditor.h"
#include "EditorWindow.h"

typedef enum assetDisplay
{
	ALL,
	MODELS,
	TEXTURES,
	SOUND,
	MUSIC,
	SCRIPTS,
	PREFAB
};

assetDisplay displayingAssets;

int displayingAsset = 0;


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
	else
	{
		try
		{
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

EditorWindow::EditorWindow()
{
#if HIDE_CONSOLE == true
	WinAPI::FreeCons();
#endif

	codeEditor = gcnew CodeEditor(this);

	strcpy(password, ENCRYPTION_PASSWORD);

	engine_bootstrap();

	dataPack = DataPacks();

	assemblies = gcnew System::Collections::Generic::List<EngineAssembly^>();

	assemblies->Add(gcnew EngineAssembly(System::Reflection::Assembly::GetExecutingAssembly()));

	for each (String ^ fileName in Directory::GetFiles("Bin\\Asm\\"))
	{
		if (fileName->Contains(".goldasm") || fileName->Contains(".dll"))
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

		case ObjectType::ModelRenderer: // MODEL RENDERER
		{
			Engine::EngineObjects::ModelRenderer^ renderer = Cast::Dynamic<Engine::EngineObjects::ModelRenderer^>(object);
			unsigned int modelId = renderer->model;
			unsigned int materialId = renderer->shader;
			unsigned int textureId = renderer->texture;
			unsigned long tint = renderer->tint;

			int mId = (int)modelId;
			int matId = (int)materialId;
			int texId = (int)textureId;

			ImGui::SeparatorText("Model Renderer");
			ImGui::Text("Model ID: ");
			ImGui::SameLine();
			if (ImGui::InputInt("###MODELID_SETTER", &mId, 1, 1))
			{
				renderer->model = (unsigned int)mId;
			}
			ImGui::Text("Shader ID: ");
			ImGui::SameLine();
			if (ImGui::InputInt("###SHADERID_SETTER", &matId, 1, 1))
			{
				renderer->shader = (unsigned int)matId;
			}
			ImGui::Text("Texture ID: ");
			ImGui::SameLine();
			if (ImGui::InputInt("###TEXTUREID_SETTER", &texId, 1, 1))
			{
				renderer->texture = (unsigned int)texId;
			}

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
				renderer->SetColorTint(ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3])));
			}

		}
		break;

		case ObjectType::LightSource:
		{
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
				Engine::Components::Vector3^ target = light->target;

				float nativeVector[3] = { light->target->x, light->target->y, light->target->z };

				ImGui::Text("Target: ");
				ImGui::SameLine();
				if (ImGui::DragFloat2("###LIGHT_TARGET", nativeVector, 1.0f, float::MinValue, float::MaxValue, "%.3f"))
				{
					light->target = gcnew Engine::Components::Vector3(nativeVector[0], nativeVector[1], nativeVector[2]);
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
							Engine::Components::Vector3^ vector = attrib->getValue<Engine::Components::Vector3^>();

							float data[3] = { vector->x, vector->y, vector->z };

							if (ImGui::DragFloat3(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.2f"))
							{
								attrib->setValue(Engine::Components::Vector3::create(data));
							}
						}
						else if (attrib->getValueType()->Equals(Engine::Components::Vector2::typeid))
						{
							Engine::Components::Vector2^ vector = (Engine::Components::Vector2^)attrib->getValue();

							float data[2] = { vector->x, vector->y };

							if (ImGui::DragFloat2(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.2f"))
							{
								attrib->setValue(Engine::Components::Vector2::create(data));
							}
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
								temp = CastStringToNative("NOT ASSIGNED - (NULL)");
							}
							else
							{
								Engine::Internal::Components::GameObject^ value = attrib->getValueAs<Engine::Internal::Components::GameObject^>();

								temp = CastStringToNative(value->name + " - (" + GetAccessRoute(value) + ")");
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
							float tmp = (double)attrib->getValue();

							float value = (double)tmp;

							if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 0.1f, 0.5f, "%.2f"))
							{
								attrib->setValue(value, false);
								attrib->setType(float::typeid);
							}
						}
						else if (attrib->getValueType()->Equals(bool::typeid))
						{
							BoolEditor(attrib);
						}
						else if (attrib->getValueType()->Equals(Engine::Components::Color::typeid))
						{
							ColorEditor(attrib);
						}

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

				auto meshRenderer = gcnew Engine::EngineObjects::ModelRenderer(
					"ModelRenderer",
					gcnew Engine::Internal::Components::Transform(
						gcnew Engine::Components::Vector3(0, 0, 0),
						gcnew Engine::Components::Vector3(0, 0, 0),
						gcnew Engine::Components::Vector3(1, 1, 1),
						nullptr
					),
					assetId,
					0,
					0,
					0xFFFFFFFF
				);
				meshRenderer->SetParent(scene->GetDatamodelMember("workspace"));
				scene->AddObjectToScene(meshRenderer);
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
		}

		if ((f->Contains(".fbx")) && (displayingAssets == ALL || displayingAssets == MODELS))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &modelTexture))
			{
				EnableFBXConverter(CastStringToNative(f));
			}
			ImGui::SameLine();
			ImGui::Text(CastStringToNative(t).c_str());
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

		if ((f->Contains(".png") || f->Contains(".jpg") || f->Contains(".bmp") || f->Contains(".hdr")) && (displayingAssets == ALL || displayingAssets == TEXTURES))
		{
			if (rlImGuiImageButton(CastStringToNative("###" + t).c_str(), &materialTexture))
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

	FileManager::WriteToCustomFile(convertedData, gcnew String(password), loadedAssets->ToArray());
}
void EditorWindow::Start()
{
	SetTraceLogLevel(LOG_DEBUG);
	SetWindowFlags(FLAG_INTERLACED_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
	OpenWindow(1280, 720, (const char*)EDITOR_VERSION);

	gcnew Engine::Managers::SignalManager();

	if (File::Exists("./Data/Keys/secrets.dat"))
	{
		auto secrets = Engine::Config::EngineSecrets::ImportSecrets("./Data/Keys/secrets.dat");

		passwd = CypherLib::GetPasswordBytes(secrets->encryptionPassword);

		auto config = Engine::Config::EngineConfiguration::ImportConfig("./Data/appinfo.dat");

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
				if (ImGui::InputText("###PASSWORD_SETTER", password, 512))
				{
					passwd = CypherLib::GetPasswordBytes(gcnew String(Engine::Config::EngineSecrets::singleton()->encryptionPassword));
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

			if (ImGui::MenuItem("Build Settings"))
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
				print("[GoldEngine]:", "Exiting PlayMode");

				EngineState::PlayMode = false;
			}
			if (ImGui::MenuItem("Game View"))
			{
				print("[GoldEngine]:", "Entering PlayMode");

				EngineState::PlayMode = true;
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
			if (ImGui::MenuItem("Empty Object"))
			{
				Engine::EngineObjects::Script^ newObject = gcnew Engine::EngineObjects::Script("Empty Object",
					gcnew Engine::Internal::Components::Transform(
						Engine::Components::Vector3::create({ 0,0,0 }),
						Engine::Components::Vector3::create({ 0,0,0 }),
						Engine::Components::Vector3::create({ 1,1,1 }),
						scene->GetDatamodelMember("workspace")->getTransform()
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
							scene->GetDatamodelMember("workspace")->getTransform()
						), 0xFFFFFFFF);

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
							nullptr
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
							nullptr
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
								gcnew Engine::Components::Vector3(0, 0, 0),
								gcnew Engine::Components::Vector3(0, 0, 0),
								gcnew Engine::Components::Vector3(1, 1, 1),
								scene->GetDatamodelMember("workspace")->getTransform()
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
						auto modelRenderer = gcnew Engine::EngineObjects::ModelRenderer(
							"ModelRenderer",
							gcnew Engine::Internal::Components::Transform(
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 0,0,0 }),
								Engine::Components::Vector3::create({ 0,0,0 }),
								scene->GetDatamodelMember("workspace")->getTransform()
							),
							0,
							0,
							0,
							0xFFFFFFFF
						);

						scene->AddObjectToScene(modelRenderer);
					}

					if (ImGui::MenuItem("MeshRenderer"))
					{

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
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(1, 1, 1),
							nullptr
						)
					);

					scene->AddObjectToScene(meshRenderer);
				}
#else
				ImGui::Text("Engine not compiled with physics module");
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
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(1, 1, 1),
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
				if (ImGui::MenuItem("Point Light"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::LightSource(
						"Point Light",
						gcnew Engine::Internal::Components::Transform(
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_POINT,
						gcnew Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
						1.0f,
						1
					);
					meshRenderer->SetParent(lightManager);

					scene->AddObjectToScene(meshRenderer);
				}

				if (ImGui::MenuItem("Dirrectional Light"))
				{
					auto meshRenderer = gcnew Engine::EngineObjects::LightSource(
						"Directional Light",
						gcnew Engine::Internal::Components::Transform(
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_DIRECTIONAL,
						gcnew Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
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
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(0, 0, 0),
							gcnew Engine::Components::Vector3(1, 1, 1),
							nullptr
						),
						0xFF0000FF,
						rPBR::LightType::LIGHT_SPOT,
						gcnew Engine::Components::Vector3(1.0f, 1.0f, 1.0f),
						1.0f,
						1
					);
					meshRenderer->SetParent(lightManager);

					scene->AddObjectToScene(meshRenderer);
				}

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
							gcnew Engine::Components::Vector3(),
							gcnew Engine::Components::Vector3(),
							gcnew Engine::Components::Vector3(1, 1, 1),
							scene->GetDatamodelMember("gui")->getTransform()
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
							gcnew Engine::Components::Vector3(),
							gcnew Engine::Components::Vector3(),
							gcnew Engine::Components::Vector3(1, 1, 1),
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
							scene->GetDatamodelMember("workspace")->getTransform()
						));

					scene->AddObjectToScene(luaScript);
				}

				ImGui::Separator();

				for each (auto assembly in assemblies)
				{
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
			if (ImGui::Checkbox("FPS", &fpsCheck))
			{
				fpsCap = fpsCheck;
			}

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
						const char* c = CastToNative(selectedObject->GetTag());

						char* objectName = new char[sizeof(c) + 8];

						strcpy(objectName, c);

						if (ImGui::InputText("Tag", objectName, selectedObject->GetTag()->Length + 8))
						{
							selectedObject->SetTag(gcnew String(objectName));
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
			char* objectName = new char[128];

			strcpy(objectName, CastToNative(selectedObject->name));

			if (ImGui::InputText("Name", objectName, 128, ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
			{
				selectedObject->name = gcnew String(objectName);
			}

			ImGui::SeparatorText("Transform");

			{
				const char* opts[2] = { "World", "Local" };

				ImGui::Combo("###POSITION_SELECTOR", &positionSelector, opts, IM_ARRAYSIZE(opts));
				if (positionSelector == 0)
				{
					// position
					float pos[3] = {
						selectedObject->getTransform()->position->x,
						selectedObject->getTransform()->position->y,
						selectedObject->getTransform()->position->z
					};

					if (ImGui::DragFloat3("Position", pos, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
					{
						selectedObject->getTransform()->position = gcnew Engine::Components::Vector3(pos[0], pos[1], pos[2]);
					}
				}
				else
				{
					// local position
					float pos[3] = {
						selectedObject->getTransform()->localPosition->x,
						selectedObject->getTransform()->localPosition->y,
						selectedObject->getTransform()->localPosition->z
					};

					if (ImGui::DragFloat3("Local Position", pos, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
					{
						selectedObject->getTransform()->UpdateLocalPosition(gcnew Engine::Components::Vector3(pos[0], pos[1], pos[2]));
					}
				}

				// rotation
				float rot[3] = {
					selectedObject->getTransform()->rotation->x,
					selectedObject->getTransform()->rotation->y,
					selectedObject->getTransform()->rotation->z
				};

				if (ImGui::DragFloat3("Rotation", rot, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
				{
					selectedObject->getTransform()->rotation = gcnew Engine::Components::Vector3(rot[0], rot[1], rot[2]);
				}

				// scale

				float scale[3] = {
					selectedObject->getTransform()->scale->x,
					selectedObject->getTransform()->scale->y,
					selectedObject->getTransform()->scale->z
				};

				if (ImGui::DragFloat3("Scale", scale, 0.01f, float::MinValue, float::MaxValue, "%.3f", ImGuiInputTextFlags_CallbackCompletion) && !readonlyLock)
				{
					selectedObject->getTransform()->scale = gcnew Engine::Components::Vector3(scale[0], scale[1], scale[2]);
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

		const char* constData[] = { "ALL", "MODELS", "TEXTURES", "SOUND", "MUSIC", "SCRIPTS", "PREFABS" };
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
void EditorWindow::DrawImGui()
{
	if (!initSettings)
	{
		ImGui::LoadIniSettingsFromDisk("imgui.ini");
		WaitTime(2.5);
		initSettings = true;
	}

	auto viewPort = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(viewPort->ID, viewPort, ImGuiDockNodeFlags_None);

	DrawMainMenuBar();

	DrawHierarchy();

	if (codeEditor->isCodeEditorOpen())
	{
		codeEditor->Draw();
	}

	DrawAssets();

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
		bool showing = showCursor;

		if (IsKeyPressed(KEY_ESCAPE))
		{
			showCursor = !showCursor;
		}

		if (!showCursor)
		{
			if (showCursor != showing)
			{
				EnableCursor();
			}
		}
		else
		{
			DisableCursor();
		}

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
		char* buffer = new char[512];

		for (int x = 0; x < loadedAssets->Count; x++)
		{
			if (buffer != "")
				delete[] buffer;

			buffer = new char[512];
			strcpy(buffer, CastToNative(loadedAssets[x]));
			std::string name = "###" + std::string(CastToNative(loadedAssets[x])) + "###_index" + std::to_string(x);
			if (ImGui::InputText(name.c_str(), buffer, 512))
			{
				loadedAssets[x] = gcnew String(buffer);
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

			delete[] buffer;

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
			char* buff = new char[8 * 512];
			strcpy(buff, CastStringToNative(config->windowName).c_str());

			ImGui::Text("Window Name: ");
			ImGui::SameLine();
			if (ImGui::InputText("##WINDOW_NAME", buff, config->windowName->Length + 512))
			{
				Engine::Config::EngineConfiguration::singleton()->windowName = gcnew String(buff);
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
		char* tmp = new char[styleFN.size() + 32] {};

		strcpy(tmp, styleFN.c_str());

		ImGui::Text("File Name:");
		ImGui::SameLine();
		if (ImGui::InputText("###FILE_NAME", tmp, styleFN.size() + 32, ImGuiInputTextFlags_CallbackCompletion))
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

		delete[] tmp;

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
		ImGui::InputText("", fileName, 1024);

		if (ImGui::Button("Create Scene"))
		{
			scene = SceneManager::CreateScene(gcnew System::String(fileName));
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
		ImGui::InputText("", fileName, 1024);

		if (ImGui::Button("Open Scene"))
		{
			SceneManager::UnloadScene(scene);
			SceneManager::LoadSceneFromFile(gcnew System::String(fileName), passwd, scene);
			//scene->LoadScene();
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
		char* data = new char[512];
		for (int x = 0; x < scene->assetPacks->Count; x++)
		{
			if (data != "")
				delete[] data;

			data = new char[512];
			strcpy(data, CastToNative(scene->assetPacks[x]));
			std::string name = "###" + std::string(CastToNative(scene->assetPacks[x]));
			if (ImGui::InputText(name.c_str(), data, 512, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				scene->assetPacks[x] = gcnew String(data);
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
	if (EngineState::PlayMode)
	{
		if (InputManager::IsKeyDown(KeyCodes::KEY_LEFT_CONTROL) && InputManager::IsKeyPressed(KeyCodes::KEY_P))
		{
			print("[GoldEngine]:", "Exiting PlayMode");

			EngineState::PlayMode = false;
		}
	}
	else
	{
		if (selectedObject != nullptr && InputManager::IsKeyDown(KeyCodes::KEY_DELETE))
		{
			Singleton<ObjectManager^>::Instance->Destroy(selectedObject);
			selectedObject = nullptr;
		}

		if (InputManager::IsKeyDown(KeyCodes::KEY_LEFT_CONTROL) && InputManager::IsKeyPressed(KeyCodes::KEY_P))
		{
			print("[GoldEngine]:", "Entering PlayMode");

			EngineState::PlayMode = true;
		}

		if (InputManager::IsKeyDown(KeyCodes::KEY_LEFT_CONTROL) && InputManager::IsKeyPressed(KeyCodes::KEY_S))
		{
			print("[GoldEngine]:", "Saving Scene");
			SceneManager::SaveSceneToFile(scene, passwd);
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

		// TODO: draw imgui over main renderer
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
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
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
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
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
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				nullptr
			)
		);

		scene->PushToRenderQueue(physicsService);
	}

#endif

	if (!scene->ExistsMember("lighting"))
	{
		lightManager = gcnew LightManager("lighting",
			gcnew Engine::Internal::Components::Transform(
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			"Data/Engine/Shaders/rPBR/pbr.vert",
			"Data/Engine/Shaders/rPBR/pbr.frag"
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
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					nullptr
				),
				"Data/Engine/Shaders/rPBR/pbr.vert",
				"Data/Engine/Shaders/rPBR/pbr.frag"
			);
		}
		lightManager->setParent(gameRoot);
		lightManager->protectMember();
	}

	if (ObjectManager::singleton()->GetChildrenOf(daemonParent)->Count <= 0)
	{
		auto lightdm = gcnew Engine::EngineObjects::Daemons::LightDaemon("lightdm",
			gcnew Engine::Internal::Components::Transform(
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				gcnew Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			lightManager
		);
		lightdm->SetParent(daemonParent);
		scene->PushToRenderQueue(lightdm);
	}

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
}
void EditorWindow::Init()
{
	modelTexture = LoadTexture("EditorAssets/Icons/Model.png");
	materialTexture = LoadTexture("EditorAssets/Icons/Material.png");
	soundTexture = LoadTexture("EditorAssets/Icons/Sound.png");
	scriptTexture = LoadTexture("EditorAssets/Icons/Script.png");

	cameraPosition = gcnew Engine::Components::Vector3(0, 0, 0);

	while (!scene->sceneLoaded())
		WaitTime(1.0);

	create();

	Logging::LogCustom("[GL Version]:", "Current OpenGL version is -> " + RLGL::rlGetVersion() + ".");

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	ImGui::GetIO().ConfigErrorRecovery = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableAssert = true;
	ImGui::GetIO().ConfigErrorRecoveryEnableDebugLog = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableTooltip = true;

	this->luaVM->ClearGlobals();
	this->luaVM->RegisterGlobalFunctions();
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

	SceneManager::LoadSceneFromFile(gcnew System::String(fileName), passwd, scene);

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
	{
		SetFPS(60);
	}
	else
	{
		SetFPS(-1);
	}

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

#pragma region PLAYBACK ENGINE

public ref class GameWindow : public Engine::Window
{
private:
	bool initSettings = false;
	System::Collections::Generic::List<EngineAssembly^>^ assemblies;
	Scene^ scene;
	DataPack^ packedData;
	Engine::Render::ScriptableRenderPipeline^ renderPipeline;

public:
	GameWindow()
	{
#if HIDE_CONSOLE == true
		WinAPI::FreeCons();
#endif

		engine_bootstrap();

		assemblies = gcnew System::Collections::Generic::List<EngineAssembly^>();
		dataPack = DataPacks();

		assemblies->Add(gcnew EngineAssembly(System::Reflection::Assembly::GetExecutingAssembly()));

		for each (String ^ fileName in Directory::GetFiles("Bin\\Asm\\"))
		{
			if (fileName->Contains(".goldasm") || fileName->Contains(".dll"))
			{
				String^ path = Directory::GetCurrentDirectory() + "\\" + fileName;

				assemblies->Add(gcnew EngineAssembly(path));
			}
		}

		SceneManager::SetAssemblyManager(assemblies);

		for each (EngineAssembly ^ assembly in assemblies)
		{
			assembly->ListAssemblyTypes();
		}

		Start();
	}

	virtual void Start() override
	{
		auto secrets = Engine::Config::EngineSecrets::ImportSecrets("./Data/Keys/secrets.dat");

		passwd = CypherLib::GetPasswordBytes(secrets->encryptionPassword);

		auto config = Engine::Config::EngineConfiguration::ImportConfig("./Data/appinfo.dat");

		config->windowFlags = config->_windowFlags->toWindowFlags();

		SetWindowFlags(config->windowFlags);
		OpenWindow(1, 1, config->getWindowName().c_str());

		if (config->resolution->x != -1 && config->resolution->y != -1)
			SetWindowPosition(config->resolution->x, config->resolution->y);
		else
			SetWindowPosition(config->resolution->w - (config->resolution->w / 2), config->resolution->h - (config->resolution->h / 2));

		SetWindowSize(config->resolution->w, config->resolution->h);

		gcnew Engine::Managers::SignalManager();

		if (!Directory::Exists(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/'))))
		{
			Directory::CreateDirectory(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/')));
		}

		gcnew Engine::Utils::LogReporter(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath);

		LayerManager::RegisterDefaultLayers();

		Preload();

		Loop();
	}

private:
	void create()
	{
		LightManager^ lightManager = nullptr;

		scene->GetDatamodelMember("workspace", true);
		scene->GetDatamodelMember("gui", true);
		auto daemonParent = scene->GetDatamodelMember("daemons");

		if (!scene->ExistsMember("lighting"))
		{
			lightManager = gcnew LightManager("lighting",
				gcnew Engine::Internal::Components::Transform(
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					nullptr
				),
				"Data/Engine/Shaders/rPBR/pbr.vert",
				"Data/Engine/Shaders/rPBR/pbr.frag"
			);

			lightManager->protectMember();

			scene->AddObjectToScene(lightManager);
		}
		else
		{
			lightManager = scene->GetMember("lighting")->ToObjectType<LightManager^>();
			lightManager->protectMember();
		}


		if (ObjectManager::singleton()->GetChildrenOf(daemonParent)->Count <= 0)
		{
			auto lightdm = gcnew Engine::EngineObjects::Daemons::LightDaemon("lightdm",
				gcnew Engine::Internal::Components::Transform(
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					gcnew Engine::Components::Vector3(0, 0, 0),
					nullptr
				),
				lightManager
			);
			lightdm->SetParent(daemonParent);
			scene->AddObjectToScene(lightdm);
		}
	}

public:
	void Init() override
	{
		while (!scene->sceneLoaded())
			WaitTime(1.0);

		//Engine::Assets::Management::DataPack::SetSingletonReference(packedData);
		create();

		Logging::LogCustom("[GL Version]:", "Current OpenGL version is -> " + RLGL::rlGetVersion() + ".");

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		ImGui::GetIO().ConfigErrorRecovery = false;
		ImGui::GetIO().ConfigErrorRecoveryEnableAssert = true;
		ImGui::GetIO().ConfigErrorRecoveryEnableDebugLog = false;
		ImGui::GetIO().ConfigErrorRecoveryEnableTooltip = true;
	}

	void Preload() override
	{
		dataPack.LoadDefaultAssets();
		renderPipeline = gcnew Engine::Render::Pipelines::LitPBR_SRP();

		SceneManager::LoadSceneFromFile(gcnew System::String(fileName), passwd, scene);

		while (!scene->sceneLoaded())
		{
			RAYLIB::WaitTime(1.0f);
		}

		packedData = scene->getSceneDataPack();

		renderPipeline = Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance;

		Init();
	}

	void DrawImGui() override
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		if (!initSettings)
		{
			ImGui::LoadIniSettingsFromDisk("imgui.ini");
			WaitTime(2.5);
			initSettings = true;
		}

		auto viewPort = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(ImGui::GetID("MAIN", "vport"), viewPort, ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
	}

	void Draw() override
	{
		renderPipeline->ExecuteRenderWorkflow(this, scene);
	}

	virtual void Update() override
	{
		if (Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance != renderPipeline)
			renderPipeline = Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance;

		for each (GameObject ^ obj in scene->GetRenderQueue())
		{
			if (obj != nullptr)
			{
				obj->GameUpdate();
			}
		}

		engine_keybinds();

		Engine::GC::EngineGC::Update();
	}

	virtual void Exit() override
	{
		DataManager::HL_FreeAll();
		Engine::Utils::LogReporter::singleton->CloseThread();
	}
};

#pragma endregion

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
		passwd = CypherLib::GetPasswordBytes(gcnew String(ENCRYPTION_PASSWORD));

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
		passwd = CypherLib::GetPasswordBytes(gcnew String(ENCRYPTION_PASSWORD));

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