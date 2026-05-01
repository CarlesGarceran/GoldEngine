#define WIN32_LEAN_AND_MEAN 
#include "CastToNative.h"
#include <string>
#include <atlbase.h>
#include <atlconv.h>
#include <msclr/marshal.h>
#include <msclr/lock.h>
#include <unordered_map>

/*
std::unordered_map<System::String^, std::string> stringCache;

std::string CastStringCached(System::String^ value)
{
	auto it = stringCache.find(value);
	if (it != stringCache.end())
		return it->second;

	System::Text::Encoding^ u8 = System::Text::Encoding::UTF8;
	array<unsigned char>^ bytes = u8->GetBytes(value);
	pin_ptr<unsigned char> pinnedPtr = &bytes[0];
	std::string native((char*)pinnedPtr);
	stringCache[value] = native;
	return native;
}
*/

const char* CastToNative(System::String^ value)
{
	msclr::interop::marshal_context ctx;
	return ctx.marshal_as<const char*>(value);
}

std::string CastStringToNative(System::String^ value)
{
	if (value == nullptr)
		return std::string();

	using namespace System::Runtime::InteropServices;

	System::IntPtr ptr = Marshal::StringToHGlobalAnsi(value);
	std::string native((char*)ptr.ToPointer());
	Marshal::FreeHGlobal(ptr);

	return native;
}

template <class returnal, class input>
returnal CastToNativeType(input value)
{
	msclr::interop::marshal_context ctx;
	return ctx.marshal_as<returnal>(value);
}