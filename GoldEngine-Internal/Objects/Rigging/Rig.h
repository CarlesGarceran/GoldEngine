#pragma once

namespace Engine::EngineObjects::Rigging
{
	ref class Bone;

	public ref class Rig : public Engine::EngineObjects::Script
	{
	private:
		System::Collections::Generic::Dictionary<String^, Engine::EngineObjects::Rigging::Bone^>^ boneMap;
		System::Collections::Generic::List<Engine::EngineObjects::Rigging::Bone^>^ bones;
		RAYLIB::Model* model;

	public:
		bool armatureCreated;
		[Engine::Scripting::PropertyAttribute] bool useGpuSkinning = true;
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ boneColor;

		Rig();

	public:
		void Awake() override;

		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;
		void DrawGizmo() override;

		void Destroy() override;

		Engine::EngineObjects::Rigging::Bone^ GetBone(String^ name);
		Engine::EngineObjects::Rigging::Bone^ GetBone(int index);

		Engine::EngineObjects::Rigging::Bone^ GetParentBone(Engine::EngineObjects::Rigging::Bone^ bone);

		RAYLIB::Model& GetModel() { return *model; }

	private:
		void CreateArmature(RAYLIB::Model& model);
	};
}