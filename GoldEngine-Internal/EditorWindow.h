#pragma once
#include "EditorTools/MaterialEditor.h"

public ref class EditorWindow : Engine::Window 
{
private:
	CodeEditor^ codeEditor;
	MaterialEditor^ materialEditor;
	Layer^ selectedLayer;
	DataPack^ packedData;
	Engine::Management::Scene^ scene;
	Engine::EngineObjects::LightManager^ lightManager;
	Engine::Components::Vector3^ cameraPosition;
	Engine::Internal::Components::GameObject^ selectedObject;
	int selectedObjectIndex;
	bool selectionLock = false;
	Engine::Internal::Components::GameObject^ selectionObject;

	

	Engine::Internal::Components::GameObject^ reparentObject;
	System::Collections::Generic::List<EngineAssembly^>^ assemblies;
	Engine::Lua::VM::LuaVM^ luaVM;
	Engine::Editor::Gui::fileExplorer^ fileExplorer = gcnew Engine::Editor::Gui::fileExplorer(std::string("File Explorer"));
	System::Collections::Generic::List<String^>^ loadedAssets;
	Engine::Render::ScriptableRenderPipeline^ renderPipeline;

public:
	System::Object^ GetSelectedObject() override { return this->selectedObject; }

public:
	void OpenFileExplorer(std::string name, Engine::Editor::Gui::explorerMode mode, Engine::Editor::Gui::onFileSelected^ callback);
	Engine::Lua::VM::LuaVM^ getLuaVM();

	EditorWindow();


private:
	void PackData(String^ convertedData);
	void DrawConsole();
	void DrawMainMenuBar();
	void DrawHierarchy();
	void DrawProperties();
	void DrawAssets();
	void ExecAsIdentifiedObject(Engine::Internal::Components::ObjectType type, System::Object^ object);
	void RegisterKeybinds();
	void create();
	void createAssetEntries(String^ path);
	void DrawHierarchyInherits(Engine::Management::Scene^ scene, Engine::Internal::Components::GameObject^ parent, int depth);
	void SpecializedPropertyEditor(Engine::Internal::Components::GameObject^ object);

public:
	void Start() override;
	void DrawImGui() override;
	void Exit() override;
	void Draw() override;
	void Init() override;
	void Preload() override;
	void Update() override;
};