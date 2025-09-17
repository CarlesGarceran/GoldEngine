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

	Layer::Layer(int mask, System::String^ name, unsigned int layerBlendFlags)
		: Layer(mask, name)
	{
		this->layerBlendFlags = layerBlendFlags;
	}

	bool Layer::IsLayer(Layer^ layerB)
	{
		if ((this->layerName->Equals(layerB->layerName)) && (this->layerMask == layerB->layerMask))
			return true;
	
		return false;
	}

	System::String^ Layer::ToString()
	{
		return layerMask.ToString() + "_" + layerName + "_" + layerBlendFlags.ToString();
	}

	void Layer::setLayerBlendFlags(unsigned int flags)
	{
		this->layerBlendFlags = flags;
	}

	unsigned int Layer::getLayerBlendFlags()
	{
		return this->layerBlendFlags;
	}
}