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

bool saveToMem = false;
bool loadFromMem = false;

unsigned int materialId;
int shaderId = 0;

const char* shaderLocType = "";

typedef enum MaterialLocations
{
	ColorLoc = 0,
	TextureLoc = 1,
	FloatLoc = 2
};


const char* materialLocations[] =
{
	"Color",
	"Texture",
	"Float"
};

void locEdit(Engine::Components::Locs::Generic::ShaderLoc^ location, Engine::Components::Locs::Generic::MaterialLoc^ locref)
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
			colorLoc->color->setHex(ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3])));
		}
		break;
	}
	case MaterialLocations::TextureLoc:
	{
		Engine::Components::Locs::TextureLoc^ texLoc = (Engine::Components::Locs::TextureLoc^)locref;
		int ptr = texLoc->textureId;
		ImGui::Text("Texture ID:");
		ImGui::SameLine();
		if (ImGui::InputInt(CastStringToNative("###TEXTURE_LOC_" + location->locName).c_str(), &ptr))
		{
			texLoc->textureId = (unsigned int)ptr;
		}
		break;
	}
	case MaterialLocations::FloatLoc:
	{
		Engine::Components::Locs::FloatLoc^ texLoc = (Engine::Components::Locs::FloatLoc^)locref;
		float ptr = texLoc->value;
		ImGui::Text("Value:");
		ImGui::SameLine();
		if (ImGui::DragFloat(CastStringToNative("###FLOAT_LOC_" + location->locName).c_str(), &ptr))
		{
			texLoc->value = (float)ptr;
		}
		break;
	}
	}
}

MaterialEditor::MaterialEditor(Engine::Window^ instance)
{
	this->instance = instance;
}

void MaterialEditor::SaveMaterial(String^ path)
{
	selectedMaterial->SerializeProperties();
	File::WriteAllText(path, Serialize(selectedMaterial));
}

void MaterialEditor::LoadMaterial(String^ path)
{
	try
	{
		String^ contents = File::ReadAllText(path);
		Engine::Components::Material^ mat = Deserialize<Engine::Components::Material^>(contents);
		mat->DeserializeProperties();
		SetMaterial(mat);
	}
	catch (System::Exception^ exception)
	{
		printError(exception->Message);
	}
}

void MaterialEditor::SaveMaterial(unsigned int id)
{
	selectedMaterial->SerializeProperties();
	DataPacks::singleton().AddMaterial(id, selectedMaterial);
}

void MaterialEditor::LoadMaterial(unsigned int id)
{
	this->SetMaterial(id);
}

void MaterialEditor::GUI()
{
	materialEditorOpen = _materialEditorOpen;

	if (materialEditorOpen)
	{
		if (ImGui::Begin("Material Editor", &_materialEditorOpen, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::BeginPopupModal("Save Material in Memory", &saveToMem))
			{
				ImGui::Text("Material ID:");
				ImGui::InputScalar("###MATERIAL_ID", ImGuiDataType_U32, &materialId);

				if (ImGui::Button("Save"))
				{
					this->SaveMaterial(materialId);
					saveToMem = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close"))
				{
					saveToMem = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("Load Material from Memory", &loadFromMem))
			{
				ImGui::Text("Material ID:");
				ImGui::InputScalar("###MATERIAL_ID", ImGuiDataType_U32, &materialId);

				if (ImGui::Button("Load"))
				{
					this->LoadMaterial(materialId);
					loadFromMem = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close"))
				{
					loadFromMem = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

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
							loadFromMem = true;
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Save Material"))
					{
						if (ImGui::MenuItem("To File"))
						{
							((EditorWindow^)instance)->OpenFileExplorer("Save File", Engine::Editor::Gui::explorerMode::Save, (gcnew Engine::Editor::Gui::onFileSelected(this, &MaterialEditor::SaveMaterial)));
						}
						if (ImGui::MenuItem("To Memory"))
						{
							saveToMem = true;
						}

						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Discard Material"))
					{
						this->selectedMaterial = nullptr;
						return;
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

			if (selectedMaterial == nullptr)
			{
				ImGui::Text("No Material Selected");
				if (ImGui::Button("Create One Now"))
				{
					SetMaterial(gcnew Engine::Components::Material(0));
				}
			}
			else
			{
				if (memEditing)
					ImGui::Text("You're editing a material that is stored in memory, therefore, changes will be applied immediatly!");

				/* SHADER ID BLOCK */
				{
					ImGui::Text("Shader Id:");
					ImGui::SameLine();
					if (ImGui::InputInt("###_SHADER_ID", &shaderId))
					{
						selectedMaterial->shaderId->setInstance(shaderId);
					}

				}

				Engine::Components::Material^ materialCopy = nullptr;
				materialCopy = selectedMaterial;

				/* MATERIAL LOCATIONS */
				ImGui::BeginTable("###MATERIAL_LOCS", 4);
				ImGui::TableSetupColumn("Index");
				ImGui::TableSetupColumn("LocationId");
				ImGui::TableSetupColumn("Value");
				ImGui::TableSetupColumn("");
				ImGui::TableHeadersRow();
				for (int x = 0; x < selectedMaterial->MaterialProperties->Count; x++)
				{
					auto location = materialCopy->MaterialProperties[x];
					Engine::Components::Locs::Generic::MaterialLoc^ locref = location->GetMaterialLocation();

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(CastStringToNative("" + x).c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(CastStringToNative(location->locName).c_str());

					ImGui::TableSetColumnIndex(2);
					{
						if (locref != nullptr)
						{
							locEdit(location, locref);
						}
					}

					ImGui::TableSetColumnIndex(3);
					if (ImGui::Button(CastStringToNative("Delete###_" + location->locName).c_str()))
					{
						selectedMaterial->RemoveProperty(location->locName);
					}
				}
				ImGui::EndTable();

				selectedMaterial = materialCopy;
			}

			if (selectedMaterial != nullptr)
			{
				if (ImGui::CollapsingHeader("Add Material Location"))
				{
					ImGui::Text("Shader Location:");
					ImGui::InputText("###LOC_ADD", &locName);

					if (ImGui::BeginCombo("###LOC_SHADER_LOCTYPE", shaderLocType))
					{
						for (int n = 0; n < IM_ARRAYSIZE(materialLocations); n++)
						{
							if (ImGui::Selectable(materialLocations[n], (n == matLocType)))
							{
								matLocType = n;
								shaderLocType = materialLocations[n];
							}
						}

						ImGui::EndCombo();
					}

					if (ImGui::Button("Add Location"))
					{
						switch (matLocType)
						{
						case MaterialLocations::ColorLoc:
							this->selectedMaterial->AddProperty(gcnew String(locName.c_str()), gcnew Engine::Components::Locs::ColorLoc(gcnew Engine::Components::Color(0xFFFFFFFF)));
							break;
						case MaterialLocations::TextureLoc:
							this->selectedMaterial->AddProperty(gcnew String(locName.c_str()), gcnew Engine::Components::Locs::TextureLoc(0));
							break;
						case MaterialLocations::FloatLoc:
							this->selectedMaterial->AddProperty(gcnew String(locName.c_str()), gcnew Engine::Components::Locs::FloatLoc(0));
							break;
						}
					}
				}
			}

			if(saveToMem)
				ImGui::OpenPopup("Save Material in Memory");

			if(loadFromMem)
				ImGui::OpenPopup("Load Material from Memory");
		}
		ImGui::End();
	}
}

void MaterialEditor::SetMaterial(Engine::Components::Material^ material)
{
	memEditing = false;
	selectedMaterial = material;
	shaderId = material->shaderId->getInstance();
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