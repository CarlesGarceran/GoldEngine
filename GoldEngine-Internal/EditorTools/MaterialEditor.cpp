#pragma managed(push,on)
#include "../SDK.h"
#include "MaterialEditor.h"

#include "../imgui/FileExplorer/filedialog.h"
#include "../LuaVM.h"

#include "../EngineIncludes.h"
#include "../ObjectManager.h"
#include "../Screen.h"

using namespace Engine::Scripting;
using namespace Engine::Components;

#include "../Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "CodeEditor.h"
#include "../EditorWindow.h"

bool _materialEditorOpen = false;
std::string locName = "";
unsigned int matLocType = 0;

bool memEditing = false;

unsigned int materialId;
int shaderId = 0;

typedef enum MaterialLocations
{
	ColorLoc = 0,
	TextureLoc = 1,
};

MaterialEditor::MaterialEditor(Engine::Window^ instance)
{
	this->instance = instance;
}

void MaterialEditor::SaveMaterial(String^ path)
{
	File::WriteAllText(path, Serialize(selectedMaterial));
}

void MaterialEditor::LoadMaterial(String^ path)
{
	try
	{
		Engine::Components::Material^ mat = Deserialize<Engine::Components::Material^>(File::ReadAllText(path));
			SetMaterial(mat);
	}
	catch (System::Exception^ exception)
	{
		printError(exception->Message);
	}
}

void MaterialEditor::GUI()
{
	materialEditorOpen = _materialEditorOpen;

	if (materialEditorOpen)
	{
		if (ImGui::Begin("Material Editor", &_materialEditorOpen, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (selectedMaterial == nullptr)
			{
				ImGui::Text("No Material Selected");
				if (ImGui::Button("Create One Now"))
				{
					selectedMaterial = gcnew Engine::Components::Material();
				}

			}
			else
			{
				{
					ImGui::BeginMenuBar();

					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::BeginMenu("Load Material"))
						{
							if (ImGui::MenuItem("From File"))
							{
								((EditorWindow^)instance)->OpenFileExplorer("Load File", Engine::Editor::Gui::explorerMode::Open, (gcnew Engine::Editor::Gui::onFileSelected(this, &MaterialEditor::LoadMaterial)));
							}
							if (ImGui::MenuItem("From Memory"))
							{

							}
							
							ImGui::EndMenu();
						}

						if (ImGui::MenuItem("Save Material"))
						{
							((EditorWindow^)instance)->OpenFileExplorer("Save File", Engine::Editor::Gui::explorerMode::Save, (gcnew Engine::Editor::Gui::onFileSelected(this, &MaterialEditor::SaveMaterial)));
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Close"))
						{
							_materialEditorOpen = false;
						}

						ImGui::EndMenu();
					}


					ImGui::EndMenuBar();
				}

				if (memEditing)
					ImGui::Text("You're editing a material that is stored in memory, therefore, changes will be applied immediatly!");

				/* SHADER ID BLOCK */
				{
					ImGui::Text("Shader Id:");
					ImGui::SameLine();
					if (ImGui::InputInt("###_SHADER_ID", &shaderId))
					{
						selectedMaterial->shaderId = shaderId;
					}

				}

				int index = 0;

				/* MATERIAL LOCATIONS */
				ImGui::BeginTable("###MATERIAL_LOCS", 4);
				ImGui::TableSetupColumn("Index");
				ImGui::TableSetupColumn("LocationId");
				ImGui::TableSetupColumn("Value");
				ImGui::TableSetupColumn("");
				ImGui::TableHeadersRow();
				for each (String ^ locKey in selectedMaterial->MaterialProperties->Keys)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(CastStringToNative("" + index).c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(CastStringToNative(locKey).c_str());

					ImGui::TableSetColumnIndex(2);
					{
						Engine::Components::Locs::Generic::MaterialLoc^ locref = nullptr;
						if (selectedMaterial->MaterialProperties->TryGetValue(locKey, locref))
						{
							if (locref != nullptr)
							{
								int locType = locref->GetLocType();

								switch (locType)
								{
								case MaterialLocations::ColorLoc:
								{
									Engine::Components::Locs::ColorLoc^ colorLoc = (Engine::Components::Locs::ColorLoc^)locref;

									unsigned int tint = colorLoc->color->toHex();

									auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(tint));

									float rawData[4] =
									{
										float4.x,
										float4.y,
										float4.z,
										float4.w
									};

									ImGui::Text("Color: ");
									ImGui::SameLine();
									if (ImGui::ColorPicker4("###TINT_SETTER", rawData))
									{
										colorLoc->color->SetHex(ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3])));
									}
									break;
								}
								case MaterialLocations::TextureLoc:
								{
									Engine::Components::Locs::TextureLoc^ texLoc = (Engine::Components::Locs::TextureLoc^)locref;
									int ptr = texLoc->textureId->Instance;
									ImGui::Text("Texture ID:");
									ImGui::SameLine();
									if (ImGui::InputInt(CastStringToNative("###TEXTURE_LOC_" + locKey).c_str(), &ptr))
									{
										texLoc->textureId->Instance = (unsigned int)ptr;
									}
									break;
								}
								}
							}
						}
					}

					ImGui::TableSetColumnIndex(3);
					if (ImGui::Button(CastStringToNative("Delete###_" + locKey).c_str()))
					{
						selectedMaterial->MaterialProperties->Remove(locKey);
					}

					index++;
				}
				ImGui::EndTable();
			}

			{
				ImGui::BeginChild("ADD_MATERIAL_LOC");

				ImGui::EndChildFrame();
			}
		}
		ImGui::End();
	}
}

void MaterialEditor::SetMaterial(Engine::Components::Material^ material)
{
	memEditing = false;
	selectedMaterial = material;
	shaderId = material->shaderId;
}

void MaterialEditor::SetMaterial(unsigned int materialId)
{
	SetMaterial(DataPacks::singleton().GetMaterial(materialId));
	memEditing = true;
}

void MaterialEditor::ShowGUI()
{
	materialEditorOpen = true;
	_materialEditorOpen = true;
}
#pragma managed(push, off)