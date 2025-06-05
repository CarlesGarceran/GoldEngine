#pragma once

namespace Engine::Components::Locs
{
	public ref class TextureLoc : Generic::MaterialLoc
	{
	public:
		unsigned int textureId;

		TextureLoc(unsigned int textureId);
		TextureLoc();

		unsigned int GetLocType() override;
	};
}