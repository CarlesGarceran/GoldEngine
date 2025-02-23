#pragma once

namespace Engine::Components::Locs
{
	public ref class TextureLoc : Generic::MaterialLoc
	{
	public:
		Reflectable::Generic::Reflectable<unsigned int>^ textureId;

		TextureLoc(unsigned int textureId);
		unsigned int GetLocType() override;

		void operator[](unsigned int textureId);
		void operator=(unsigned int textureId);
	};

}