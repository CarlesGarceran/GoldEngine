#pragma managed(push,on)
#include "../SDK.h"

#if (!PRODUCTION_BUILD)

#include "../imgui/FileExplorer/filedialog.h"
#include "CodeEditor.h"
#include "../LuaVM.h"

#include "../EngineIncludes.h"
#include "../ObjectManager.h"
#include "../Screen.h"

using namespace Engine::Scripting;
using namespace Engine::Components;

#include "../Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "../EditorWindow.h"

using namespace System;
using namespace System::IO;

/* SCRIPTING TEMPLATES */

std::string luaTemplate = std::string(R"(-- Called when the object gets instantiated
function Start()

end

-- Called each time the physics engine does a tick (1/2 of the normal framerate by default).
function PhysicsUpdate()

end

-- Called every frame (after draw).
function Update()

end

-- Called every frame (before update).
function Draw()

end

-- Called when the ImGUI execution cycle is began (allows for using the imgui hook)
function DrawImGUI()

end

-- Called every frame (Renders only on editor (not client)).
function DrawGizmos()

end
)");

std::string cppcliTemplate = std::string(R"(#include <cstdio>
#include <Windows.h>
#include "Typedefs.h"

using namespace Engine::EngineObjects;
using namespace Engine::Management;
using namespace Engine::Managers;
using namespace Engine::Scripting;
using namespace Engine::Internal::Components;

namespace UserScripts
{
	public ref class NewBehaviour : public Engine::EngineObjects::Script
	{
	public:
		NewBehaviour(System::String^ name, Engine::Internal::Components::Transform^ transform) : Script(name, transform)
		{

		}

	public:
		void Update() override
		{

		}

		void Draw() override
		{

		}

		void DrawGizmo() override
		{

		}

		void DrawImGUI() override
		{

		}
	};
}
)");

/* END SCRIPTING TEMPLATES */

void onEditorTabsDispose(std::vector<EditorTab> tabs)
{

}

bool codeEditorOpen = false;

CodeEditor::CodeEditor(Engine::Window^ instance)
{
	std::vector<EditorTab> editorTabs = std::vector<EditorTab>();

	EditorTab defaultTab = EditorTab();
	defaultTab.codeEditorFile = "Untitled.txt";

	editorTabs.push_back(defaultTab); // DEFAULT TAB
	bool value = false;
	tabs = new Engine::Native::EnginePtr<std::vector<EditorTab>>(editorTabs, &onEditorTabsDispose);

	this->instance = instance;

	try
	{
		EditorWindow^ window = (EditorWindow^)instance;
	}
	catch (System::Exception^ ex)
	{
		delete this;
	}
}

void CodeEditor::SaveEditorCode()
{
	if (tabs->getInstance()[selectedTab].codeEditorFile != "")
	{
		File::WriteAllText(
			gcnew String(tabs->getInstance()[selectedTab].codeEditorFile.c_str()),
			gcnew String(tabs->getInstance()[selectedTab].codeEditor.GetText().c_str()));
	}
}

void CodeEditor::Draw()
{
	if (ImGui::Begin("Embedded Code Editor", &codeEditorOpen, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					EditorTab defaultTab = EditorTab();
					defaultTab.codeEditorFile = "Untitled.txt";

					tabs->getInstance().push_back(defaultTab);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Open"))
				{
					if (tabs->getInstance()[selectedTab].codeEditorFile != "")
					{
						String^ chunk = gcnew String(tabs->getInstance()[selectedTab].codeEditorChunk.c_str());
						String^ currentChunk = gcnew String(tabs->getInstance()[selectedTab].codeEditor.GetText().c_str());

						if (!currentChunk->Equals(chunk))
						{
							tabs->getInstance()[selectedTab].ce1 = true;
						}
						else
						{
							((EditorWindow^)instance)->OpenFileExplorer("Open File", Engine::Editor::Gui::explorerMode::Open, (gcnew Engine::Editor::Gui::onFileSelected(this, &CodeEditor::SetEditorCode)));
						}

						delete chunk;
						delete currentChunk;
					}
					else
					{
						((EditorWindow^)instance)->OpenFileExplorer("Open File", Engine::Editor::Gui::explorerMode::Open, (gcnew Engine::Editor::Gui::onFileSelected(this, &CodeEditor::SetEditorCode)));
					}
				}
				if (ImGui::MenuItem("Save"))
				{
					tabs->getInstance()[selectedTab].codeEditorChunk = tabs->getInstance()[selectedTab].codeEditor.GetText();
					SaveEditorCode();
				}
				if (ImGui::MenuItem("Save As"))
				{
					tabs->getInstance()[selectedTab].codeEditorChunk = tabs->getInstance()[selectedTab].codeEditor.GetText();

					((EditorWindow^)instance)->OpenFileExplorer("Save File", Engine::Editor::Gui::explorerMode::Save, (gcnew Engine::Editor::Gui::onFileSelected(this, &CodeEditor::SaveEditorContents)));
				}
				ImGui::Separator();

				if (ImGui::MenuItem("Close", "", (bool*)0, (tabs->getInstance().size() > 1)))
				{
					if (tabs->getInstance().size() > 1)
					{
						tabs->getInstance().erase(tabs->getInstance().begin() + selectedTab);
						if (selectedTab != 0)
							selectedTab--;
						else
							selectedTab = 0;
					}
				}

				if (ImGui::MenuItem("Exit"))
				{
					codeEditorOpen = false;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Compiler"))
			{
				if (ImGui::BeginMenu("Lua Compiler"))
				{
					if (ImGui::MenuItem("Save Lua Bytecode"))
					{
						((EditorWindow^)instance)->getLuaVM()->LoadSource(gcnew String(tabs->getInstance()[selectedTab].codeEditor.GetText().c_str()));

						((EditorWindow^)instance)->OpenFileExplorer("Save Lua Compiled File", Engine::Editor::Gui::explorerMode::Save, (gcnew Engine::Editor::Gui::onFileSelected(((EditorWindow^)instance)->getLuaVM(), &Engine::Lua::VM::LuaVM::WriteLuaCodeToFile)));
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Language"))
			{
				if (ImGui::MenuItem("C++"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::CPlusPlus();
				}
				if (ImGui::MenuItem("C#"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::Cs();
				}
				if (ImGui::MenuItem("GLSL"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::GLSL();
				}
				if (ImGui::MenuItem("Lua"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::Lua();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Templates"))
			{
				if (ImGui::MenuItem("C++ Script Template"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::CPlusPlus();
					tabs->getInstance()[selectedTab].codeEditor.SetText(cppcliTemplate);
				}
				if (ImGui::MenuItem("GLSL"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::GLSL();
				}
				if (ImGui::MenuItem("Lua Script Template"))
				{
					tabs->getInstance()[selectedTab].language = TextEditor::LanguageDefinition::Lua();
					tabs->getInstance()[selectedTab].codeEditor.SetText(luaTemplate);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (tabs->getInstance()[selectedTab].ce1)
		{
			ImGui::OpenPopup("Code Editor - Save Changes");
		}

		if (ImGui::BeginPopupModal("Code Editor - Save Changes", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("You have unsaved changes, do you want to save them?");

			if (ImGui::Button("Yes"))
			{
				tabs->getInstance()[selectedTab].ce1 = false;
				SaveEditorCode();
				tabs->getInstance()[selectedTab].codeEditorChunk = tabs->getInstance()[selectedTab].codeEditor.GetText();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				tabs->getInstance()[selectedTab].ce1 = false;
				tabs->getInstance()[selectedTab].codeEditorChunk = tabs->getInstance()[selectedTab].codeEditor.GetText();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginTabBar("TAB_MODULE", ImGuiTabBarFlags_None))
		{
			for (int x = 0; x < tabs->getInstance().size(); x++)
			{
				if (ImGui::TabItemButton((tabs->getInstance()[x].codeEditorFile + "###" + std::to_string(x)).c_str()))
				{
					selectedTab = x;
				}
			}
			ImGui::EndTabBar();
		}

		tabs->getInstance()[selectedTab].codeEditor.SetLanguageDefinition(tabs->getInstance()[selectedTab].language);
		tabs->getInstance()[selectedTab].codeEditor.Render(TextFormat("Embedded_Code_Editor_%d", selectedTab));
	}

	if (codeEditorOpen)
		ImGui::End();
}

void CodeEditor::SetEditorCode(String^ file)
{
	SaveEditorCode();

	file = file->Replace("\\", "/");
	if (file->LastIndexOf('/') != -1)
		tabs->getInstance()[selectedTab].codeEditorFile = CastStringToNative(file->Substring(file->LastIndexOf('/') + 1));
	else
		tabs->getInstance()[selectedTab].codeEditorFile = CastStringToNative(file);

	String^ content = File::ReadAllText(file);

	if (content->Equals(""))
	{
		tabs->getInstance()[selectedTab].codeEditor.SetText("");
		tabs->getInstance()[selectedTab].codeEditorChunk = "";
	}
	else
	{
		tabs->getInstance()[selectedTab].codeEditor.SetText(CastStringToNative(content));
		tabs->getInstance()[selectedTab].codeEditorChunk = CastStringToNative(content);
	}
}

void CodeEditor::SaveEditorContents(String^ path)
{
	path = path->Replace("\\", "/");
	if (path->LastIndexOf('/') != -1)
		tabs->getInstance()[selectedTab].codeEditorFile = CastStringToNative(path->Substring(path->LastIndexOf('/') + 1));
	else
		tabs->getInstance()[selectedTab].codeEditorFile = CastStringToNative(path);

	StreamWriter^ streamWriter = gcnew StreamWriter(path);

	streamWriter->Write(gcnew String(tabs->getInstance()[selectedTab].codeEditorChunk.c_str()));
	streamWriter->Flush();

	streamWriter->Close();
}

void CodeEditor::SetEditorCode(std::string fileName, std::string fileContents)
{
	SaveEditorCode();

	std::ofstream stream(tabs->getInstance()[selectedTab].codeEditorFile.c_str());

	if (stream.good())
	{
		stream.clear();
		stream << tabs->getInstance()[selectedTab].codeEditor.GetText();
	}

	stream.close();

	tabs->getInstance()[selectedTab].codeEditorFile = fileName;
	tabs->getInstance()[selectedTab].codeEditor.SetText(fileContents.c_str());
	tabs->getInstance()[selectedTab].codeEditorChunk = fileContents.c_str();
}


void CodeEditor::createTab(String^ file, TextEditor::LanguageDefinition def)
{
	EditorTab newTab = EditorTab();

	newTab.language = def;

	tabs->getInstance().push_back(newTab);
	selectedTab = tabs->getInstance().size() - 1;

	SetEditorCode(file);
}

void CodeEditor::setCodeEditorOpen(bool value)
{
	codeEditorOpen = value;
}

bool CodeEditor::isCodeEditorOpen()
{
	return codeEditorOpen;
}


#endif


#pragma managed(pop)