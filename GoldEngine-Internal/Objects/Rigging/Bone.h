#pragma once

namespace Engine::EngineObjects::Rigging
{
	ref class Rig;

	public ref class Bone sealed : public Engine::EngineObjects::Script
	{
	public:
		Engine::Scripting::InstanceReference<
			Engine::Internal::Components::GameObject^
		> rig;

	public:
		Bone();

		void DrawGizmo() override;
		void Destroy() override;
		Engine::EngineObjects::Rigging::Rig^ GetRig();

	internal:
		Bone(Engine::EngineObjects::Rigging::Rig^ rig);
	};
}