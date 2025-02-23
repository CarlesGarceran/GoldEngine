#pragma once


namespace Engine::Components
{
	namespace Locs
	{
		public ref class ColorLoc : Generic::MaterialLoc
		{
		public:
			Engine::Reflectable::Generic::Reflectable<Engine::Components::Color^>^ color;

			ColorLoc(Engine::Components::Color^ color);
			void SetColor(Engine::Components::Color^ color);
			unsigned int GetLocType() override;
		};
	}
}