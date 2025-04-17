#include "Windows.h"
#include <string>
#include <iostream>
#include "WinAPI.h"

BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM param)
{
    char windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        std::cout << "Window Title: " << windowTitle << std::endl;
        std::cout << "Top-left: (" << rect.left << ", " << rect.top << ")" << std::endl;
        std::cout << "Bottom-right: (" << rect.right << ", " << rect.bottom << ")" << std::endl;
        std::cout << "--------------------------" << std::endl;
    }

    return TRUE;
}



void WinAPI::MBOXA(void* winHandle, const char* contents, const char* owner, unsigned int flags)
{
	MessageBoxA((HWND)winHandle, contents, owner, flags);
}

void WinAPI::FreeCons()
{
	FreeConsole();
}

void WinAPI::AllocCons()
{
	AllocConsole();
}

void WinAPI::SetWindowStatus(void* winHandle, int windowId)
{
	ShowWindow((HWND)winHandle, windowId);
}

void WinAPI::SetAttribute(const char* path, int ptr)
{
#if _DEBUG
	SetFileAttributes((LPCWSTR)path, (DWORD)ptr);
#else
	SetFileAttributesA((LPCSTR)path, (DWORD)ptr);
#endif
}

void WinAPI::LoadLib(const char* libName)
{
    LoadLibrary(libName);
}