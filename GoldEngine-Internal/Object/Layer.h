#pragma once

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Layer
	{
	public:
		int layerMask; // int that represents the priority on rendering (layer 1 will write over layer 0, layer 2 will write over layer 1 and 0, ...);
		System::String^ layerName;

		unsigned int layerBlendFlags;

	public:
		Layer();
		Layer(int mask, System::String^ name);
		Layer(int mask, System::String^ name, unsigned int layerBlendFlags);

		void setLayerBlendFlags(unsigned int flags);
		unsigned int getLayerBlendFlags();

		bool IsLayer(Layer^ layerB);

		System::String^ ToString() override;
	};
}
