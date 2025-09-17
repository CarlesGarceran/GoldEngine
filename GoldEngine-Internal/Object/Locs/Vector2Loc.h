#pragma once

namespace Engine::Components::Locs
{
	public ref class Vector2Loc : Generic::MaterialLoc
	{
	public:
		Engine::Components::Vector2 value;

		Vector2Loc(Engine::Components::Vector2);
		Vector2Loc();

		unsigned int GetLocType() override;
	};
}