#pragma once

using namespace System;

struct EditorTab
{
public:
	TextEditor codeEditor = TextEditor();
	std::string codeEditorChunk = "";
	TextEditor::LanguageDefinition language = TextEditor::LanguageDefinition::CPlusPlus();
	std::string codeEditorFile = "";

	bool ce1;
};

public ref class CodeEditor
{
private:
	int selectedTab = 0;
	Engine::Native::EnginePtr<std::vector<EditorTab>>* tabs;
	Engine::Window^ instance;

public:
	CodeEditor(Engine::Window^ instance);

public:
	void Draw();

	void createTab(String^ file, TextEditor::LanguageDefinition langDef);

	void SetEditorCode(String^ file);
	void SetEditorCode(std::string fileName, std::string fileContents);

	void SaveEditorContents(String^ path);
	void SaveEditorCode();

	void setCodeEditorOpen(bool value);
	bool isCodeEditorOpen();
};