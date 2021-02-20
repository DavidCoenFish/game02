#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#include <windows.h>

#include <wrl/client.h>
#include <wrl/event.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <array>
#include <algorithm>
#include <codecvt>
#include <cmath>
#include <exception>
#include <functional>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>



#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <stdio.h>
#include <string>
#include <map>
#include <variant>
#include <vector>

// To use graphics and CPU markup events with the latest version of PIX, change this to include <pix3.h>
// then add the NuGet package WinPixEventRuntime to the project.
#include <pix.h>

#include "Common/Macro.h"
#include "Common/d3dx12.h"

#define SQRT_0_75 0.86602540378443864676372317075294f

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        virtual const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }

    // Helper for output debug tracing
    inline void DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...) noexcept
    {
    #ifdef _DEBUG
        va_list args;
        va_start(args, format);

        char buff[1024] = {};
        vsprintf_s(buff, format, args);
        OutputDebugStringA(buff);
        va_end(args);
    #else
        UNREFERENCED_PARAMETER(format);
    #endif
    }
}

namespace DirectX
{
   struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); } };

   struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

}