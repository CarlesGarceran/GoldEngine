#pragma once

template <typename T>
public struct NativeSingleton
{
private:
	inline static void* instance;
	inline static bool allocated = false;

public:
	inline static T& get()
	{
		return *(T*)instance;
	}

	inline static void create(const T& arg)
	{
		instance = arg;
		allocated = true;
	}

	inline static void create(T* arg)
	{
		instance = arg;
		allocated = true;
	}

	inline static T& free()
	{
		T& arg = *((T*)instance);
		instance = nullptr;
		allocated = false;

		return arg;
	}

	// Name Renames (for PASCAL name convention)
	static T Get() { return get(); }
	static void Create(T* arg) { create(arg); }
	static void Create(const T& arg) { create(arg); }
	static T& Free() { return free(); }
};