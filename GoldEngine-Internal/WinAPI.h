#pragma once
public ref class WinAPI
{
public:
	static void MBOXA(void* winHandle, const char* contents, const char* owner, unsigned int flags);
	static void FreeCons();
	static void AllocCons();
	static void SetWindowStatus(void* winHandle, int windowId);
	static void SetAttribute(const char* path, int ptr);


	static void LoadLib(const char* libraryName);
};

