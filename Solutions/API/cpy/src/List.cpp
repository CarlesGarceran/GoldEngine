#include "List.h"
#include <vector>
#include <map>

#define LOG_OUTPUT

#ifdef LOG_OUTPUT
#define LOG printf
#else
void stub(const char*) {}

#define LOG stub
#endif

std::vector<Sampler>* ToVector(void* data)
{
	if (!data) {
		LOG("Error: null data pointer in ToVector");
		return nullptr;
	}
	return static_cast<std::vector<Sampler>*>(data);
}

extern "C"
{
	List* CreateEmptyList(size_t elementSize)
	{
		return CreateList(elementSize, 0);
	}

	List* CreateList(size_t elementSize, size_t initialCapacity)
	{
		List* list = new List();
		list->element_size = elementSize;
		list->capacity = initialCapacity;
		list->inner_ref = (void*)new std::vector<Sampler>();
		list->defaultValue = { TEX2D, 0 };

		for (int i = 0; i < initialCapacity; i++)
		{
			ToVector(list->inner_ref)->push_back(list->defaultValue);
		}

		return list;
	}

	void SetFallbackValue(List* list, Sampler defaultValue)
	{
		if (!list) return;

		list->defaultValue = defaultValue;
	}

	void* GetData(List* list)
	{
		if (!list) return NULL;

		try
		{
			return ToVector(list->inner_ref)->data();
		}
		catch (const std::exception& ex)
		{
			LOG(ex.what());
			return nullptr;
		}
	}

	Sampler GetAt(List* list, int index)
	{
		if (!list || index < 0) 
		{
			LOG("Error: Invalid index");
			return { TEX2D, 0 };  // Return default if index is invalid
		}

		try 
		{
			return ToVector(list->inner_ref)->at(index); // Throws out_of_range on invalid index
		}
		catch (const std::exception& ex) 
		{
			LOG(ex.what());
			return { TEX2D, 0 }; // Return default value
		}
	}

	void PushBack(List* list, Sampler data)
	{
		if (!list) return;

		try
		{
			auto vector = ToVector(list->inner_ref);

			if (vector->size() + 1 > list->capacity) return;

			ToVector(list->inner_ref)->push_back(data);
		}
		catch (const std::exception& ex)
		{
			LOG(ex.what());
		}
	}

	void Emplace(List* list, int index, Sampler data)
	{
		if (!list) return;

		try
		{
			auto vector = ToVector(list->inner_ref);
			if (index < 0 || index > vector->size()) return;

			vector->emplace(vector->begin() + index, data);
		}
		catch (const std::exception& ex)
		{
			LOG(ex.what());
		}
	}

	void SetAt(List* list, int index, Sampler data)
	{
		if (!list) return;

		try
		{
			auto vector = ToVector(list->inner_ref);
			if (index < 0) return;

			if (index >= (int)vector->size())
				vector->resize(index + 1, list->defaultValue); // resize and fill new slots with nullptr

			(*vector)[index] = data;
		}
		catch (const std::exception& ex)
		{
			LOG(ex.what());
		}
	}

	void DestroyList(List* list)
	{
		if (!list) return;

		delete ToVector(list->inner_ref);

		delete list;
	}

	size_t SizeOf(List* list)
	{
		if (!list) return 0;

		try
		{
			return ToVector(list->inner_ref)->size();
		}
		catch (const std::exception& ex)
		{
			LOG(ex.what());
			return 0;
		}
	}
}