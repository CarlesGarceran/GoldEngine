#pragma once

public ref class MaterialEditor
{
private:
	Engine::Components::Material^ selectedMaterial;
	bool materialEditorOpen = false;
	Engine::Window^ instance;

public:
	MaterialEditor(Engine::Window^ instance);

	void SetMaterial(Engine::Components::Material^);
	void SetMaterial(unsigned int);
	void SaveMaterial(String^ path);
	void LoadMaterial(String^ path);
	void SaveMaterial(unsigned int id);
	void LoadMaterial(unsigned int id);
	void GUI();
	void ShowGUI();

	bool isMaterialEditorOpen()
	{
		return materialEditorOpen;
	}
};