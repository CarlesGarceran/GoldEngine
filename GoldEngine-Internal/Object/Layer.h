#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Layer
	{
	public:
		int layerMask; // int that represents the priority on rendering (layer 1 will write over layer 0, layer 2 will write over layer 1 and 0, ...);
		System::String^ layerName;

		unsigned short layerBlendFlags;

	public:
		Layer();
		Layer(int mask, System::String^ name);
		Layer(int mask, System::String^ name, unsigned short layerBlendFlags);

		void setLayerBlendFlags(unsigned short flags);
		unsigned short getLayerBlendFlags();

		bool IsLayer(Layer^ layerB);

		System::String^ ToString() override;
	};
}
