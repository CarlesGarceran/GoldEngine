#pragma once

namespace Engine::Components::Locs
{
	public ref class FloatLoc : Generic::MaterialLoc
	{
	public:
		float value;

		FloatLoc(float value);
		FloatLoc();

		void SetValue(float value);
		unsigned int GetLocType() override;
	};
}