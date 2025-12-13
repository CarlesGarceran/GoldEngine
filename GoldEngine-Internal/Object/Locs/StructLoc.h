#pragma once

namespace Engine::Components::Locs
{
	public ref class StructLoc : Generic::MaterialLoc
	{
	private:
		Engine::Reflectable::Generic::Reflectable<System::Object^>^ structure;

	public:
		StructLoc(System::Object^ structPointer);
		StructLoc();

		System::Object^ getInstance();
		void setInstance(System::Object^% instance);

		unsigned int GetLocType() override;

		System::Object^ GetValue() override;
		void SetValue(System::Object^) override;
	};
}