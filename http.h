#pragma once
#include <aclapi.h>
#include <commctrl.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

bool SendHttpRequest(const wchar_t* hostname, const wchar_t* path, const wchar_t* method, const wchar_t* headers, const char* body, size_t bodyLen, void* buffer, size_t maxlen, bool useSSL = true);