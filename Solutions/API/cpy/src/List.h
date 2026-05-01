#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		TEX2D,
		CUBEMAP
	} SamplerType;

	typedef struct
	{
		SamplerType samplerType;
		unsigned int texId;
	} Sampler;

	typedef struct
	{
		size_t element_size;
		size_t capacity;
		void* inner_ref;

		Sampler defaultValue;
	} List;

	List* CreateEmptyList(size_t elementSize);
	List* CreateList(size_t elementSize, size_t initialCapacity);
	void SetFallbackValue(List* list, Sampler defaultValue);
	void* GetData(List* list);
	Sampler GetAt(List* list, int index);
	void PushBack(List* list, Sampler data);
	void Emplace(List* list, int index, Sampler data);
	void SetAt(List* list, int index, Sampler data);

	size_t SizeOf(List* list);

	void DestroyList(List* list);

#ifdef __cplusplus
}
#endif

#endif