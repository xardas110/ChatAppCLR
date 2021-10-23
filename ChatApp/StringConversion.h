#pragma once
#include <string>

inline void MarshalString(System::String^ s, std::string& os)
{
    using namespace System::Runtime::InteropServices;
    const char* chars =
        (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
    os = chars;
    Marshal::FreeHGlobal(System::IntPtr((void*)chars));
}

inline void MarshalString(System::String^ s, std::wstring& os) {
    using namespace System::Runtime::InteropServices;
    const wchar_t* chars =
        (const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
    os = chars;
    Marshal::FreeHGlobal(System::IntPtr((void*)chars));
}
