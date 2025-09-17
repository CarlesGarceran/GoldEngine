#pragma once

public ref class GameWindow : public Engine::Window
{
private:
	bool initSettings = false;
	System::Collections::Generic::List<EngineAssembly^>^ assemblies;
	Engine::Management::Scene^ scene;
	Engine::Assets::Management::DataPack^ packedData;
	Engine::Render::ScriptableRenderPipeline^ renderPipeline;

public:
	GameWindow();

	void Start() override;
	void Update() override;
	void Draw() override;
	void Exit() override;

	void Init() override;
	void Preload() override;
	void DrawImGui() override;

private:
	void create();
};