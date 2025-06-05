#pragma once

namespace Engine::Components::Locs
{
	public ref class ColorLoc : Generic::MaterialLoc
	{
	public:
		Engine::Components::Color^ color;

		ColorLoc(Engine::Components::Color^ color);
		ColorLoc();

		void SetColor(Engine::Components::Color^ color);
		unsigned int GetLocType() override;
	};
}