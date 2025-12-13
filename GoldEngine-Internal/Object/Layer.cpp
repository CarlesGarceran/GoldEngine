#include "../Macros.h"
#include "Layer.h"

using namespace Engine::Components;

namespace Engine::Components 
{
	Layer::Layer()
	{

	}

	Layer::Layer(int mask, System::String^ name)
	{
		this->layerMask = mask;
		this->layerName = name;
	}

	Layer::Layer(int mask, System::String^ name, unsigned short layerBlendFlags)
		: Layer(mask, name)
	{
		this->layerBlendFlags = layerBlendFlags;
	}

	bool Layer::IsLayer(Layer^ layerB)
	{
		if (this == nullptr) return false;
		if (layerB == nullptr) return false;

		if ((this->layerName->Equals(layerB->layerName)) && (this->layerMask == layerB->layerMask))
			return true;
	
		return false;
	}

	System::String^ Layer::ToString()
	{
		return layerMask.ToString() + "_" + layerName + "_" + layerBlendFlags.ToString();
	}

	void Layer::setLayerBlendFlags(unsigned short flags)
	{
		this->layerBlendFlags = flags;
	}

	unsigned short Layer::getLayerBlendFlags()
	{
		return this->layerBlendFlags;
	}
}