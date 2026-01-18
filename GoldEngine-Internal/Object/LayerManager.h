#pragma once

using namespace System;
using namespace Engine::Components;

#include "../CastToNative.h"
#include "../Event.h"

namespace Engine::Scripting
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class LayerManager abstract
	{
	private:
		static System::Collections::Generic::List<Layer^>^ layers;

	private:
		static Layer^ CreateLayer(unsigned int layerId, System::String^ layerName, unsigned int layerFlags)
		{
			return gcnew Layer(layerId, layerName, layerFlags);
		}

	public:
		static Engine::Scripting::Events::Event^ onLayerAdded = gcnew Engine::Scripting::Events::Event();
		static Engine::Scripting::Events::Event^ onLayerRemoved = gcnew Engine::Scripting::Events::Event();

	public:
		static void RegisterDefaultLayers()
		{
			layers = gcnew System::Collections::Generic::List<Layer^>();

			AddLayer(CreateLayer(0, "EngineRoot", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(1, "Geometry", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(2, "Transparent", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(3, "PostFX", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(4, "Triggers", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(5, "Colliders", RAYLIB::BLEND_ALPHA));
			AddLayer(CreateLayer(6, "PhysicsWorld", RAYLIB::BLEND_ALPHA));
		}

		static void ClearLayers()
		{
			layers->Clear();
		}

		static System::String^ SerializeLayers()
		{
			return Newtonsoft::Json::JsonConvert::SerializeObject(layers);
		}

		static void LoadLayers(System::String^ _layers)
		{
			layers = (System::Collections::Generic::List<Layer^>^)Newtonsoft::Json::JsonConvert::DeserializeObject<System::Collections::Generic::List<Layer^>^>(_layers);
			sortLayers();
		}

		static void LoadLayers(System::Collections::Generic::List<Layer^>^ _layers)
		{
			layers = _layers;
			sortLayers();
		}

	public:
		static std::vector<std::string> getLayerNames()
		{
			std::vector<std::string> slayers = std::vector<std::string>();

			for (int x = 0; x < layers->Count; x++)
			{
				slayers.push_back(CastStringToNative(layers[x]->layerMask + " - " + layers[x]->layerName));
			}

			return slayers;
		}

		static std::vector<const char*> getLayerNames_char()
		{
			std::vector<const char*> slayers = std::vector<const char*>();

			for (int x = 0; x < layers->Count; x++)
			{
				slayers.push_back(CastStringToNative(layers[x]->layerMask + " - " + layers[x]->layerName).c_str());
			}

			return slayers;
		}

	public:
		static Layer^ GetLayerFromId(int id)
		{
			for each (Layer ^ l in layers)
			{
				if (l->layerMask == id)
				{
					return l;
				}
			}

			return nullptr;
		}

	public:
		static Layer^ GetLayerFromName(String^ id)
		{
			for each (Layer ^ l in layers)
			{
				if (l->layerName->Equals(id) || l->layerName->CompareTo(id) <= 0 || l->layerName == id)
				{
					return l;
				}
			}

			return nullptr;
		}


	public:
		static void AddLayer(Layer^ layer)
		{
			if (layer == nullptr)
			{
				//printError("Attempt to add a null layer to the stack.");
				return;
			}

			if (GetLayerFromId(layer->layerMask) != nullptr)
			{
				//printError("Attempt to add a repeated layer with ID: " + layer->layerMask);
				return;
			}

			layers->Add(layer);
			sortLayers();
			onLayerAdded->raiseExecution(gcnew cli::array<Engine::Components::Layer^>(1) { layer });
		}

		static void RemoveLayer(Layer^ layer)
		{
			if (GetLayerFromId(layer->layerMask) != nullptr)
			{
				if (layers->Contains(layer))
				{
					layers->Remove(layer);
				}
				else
				{
					layers->Remove(GetLayerFromId(layer->layerMask));
				}

				sortLayers();
				onLayerRemoved->raiseExecution(gcnew cli::array<Engine::Components::Layer^>(1) { layer });
			}
		}

		static void RemoveLayer(unsigned int layerId)
		{
			if (GetLayerFromId(layerId) != nullptr)
			{
				auto layer = GetLayerFromId(layerId);

				if (layers->Contains(layer))
				{
					layers->Remove(layer);
				}
				else
				{
					layers->Remove(GetLayerFromId(layer->layerMask));
				}

				sortLayers();
				onLayerRemoved->raiseExecution(gcnew cli::array<Engine::Components::Layer^>(1) { layer });
			}
		}

	public:
		static Layer^ getNextHigherLayer(Layer^ currentLayer)
		{
			for each (Layer ^ l in layers)
			{
				if (l->layerMask > (currentLayer->layerMask))
				{
					return l;
				}
			}

			return nullptr;
		}

	public:
		static System::Collections::Generic::List<Layer^>^ GetLayers() 
		{ 
			return layers;
		}

	public:
		static int getHigherLayer()
		{
			int layerLevel = int::MinValue;

			for each (Layer ^ l in layers)
			{
				if (l->layerMask > layerLevel)
				{
					layerLevel = l->layerMask;
				}
			}

			return layerLevel;
		}

		static int getLowerLayer()
		{
			int layerLevel = int::MaxValue;

			for each (Layer ^ l in layers)
			{
				if (l->layerMask < layerLevel)
				{
					layerLevel = l->layerMask;
				}
			}

			return layerLevel;
		}

	private:
		static void sortLayers()
		{
			int n = layers->Count;
			bool swapped;

			do
			{
				swapped = false;
				for (int i = 0; i < n - 1; ++i)
				{
					Layer^ currentLayer = layers[i];
					Layer^ nextLayer = layers[i + 1];

					if (currentLayer->layerMask > nextLayer->layerMask)
					{
						layers[i] = nextLayer;
						layers[i + 1] = currentLayer;
						swapped = true;
					}
				}
				--n;
			} while (swapped);
		}
	};
}