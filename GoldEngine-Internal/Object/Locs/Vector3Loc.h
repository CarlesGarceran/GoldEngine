#pragma once

namespace Engine::Components::Locs
{
	public ref class Vector3Loc : Generic::MaterialLoc
	{
	public:
		Engine::Components::Vector3 value;

		Vector3Loc(Engine::Components::Vector3);
		Vector3Loc();

		unsigned int GetLocType() override;
	};
}