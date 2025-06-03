#include "http.h"

bool SendHttpRequest(const wchar_t* hostname, const wchar_t* path, const wchar_t* method, const wchar_t* headers, const char* body, size_t bodyLen, void* buffer, size_t maxlen, bool useSSL)
{
    HINTERNET hSession = 0, hConnect = 0, hRequest = 0;
    DWORD len, offset = 0;
    char* buf = (char*)buffer;
    bool success = false;

    hSession = WinHttpOpen(L"", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession) {
        INTERNET_PORT port = useSSL ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
        hConnect = WinHttpConnect(hSession, hostname, port, 0);
    }

    if (hConnect) {
        DWORD flags = useSSL ? WINHTTP_FLAG_SECURE : 0;
        hRequest = WinHttpOpenRequest(hConnect, method, path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    }

    if (hRequest) {
        DWORD headersLen = headers ? wcslen(headers) : 0;
        if (WinHttpSendRequest(hRequest, headers, headersLen,
            (LPVOID)body, bodyLen, bodyLen, 0) &&
            WinHttpReceiveResponse(hRequest, NULL)) {
            while (WinHttpQueryDataAvailable(hRequest, &len) && len && offset < maxlen - 1) {
                DWORD bytesToRead = min(maxlen - offset - 1, len);
                if (WinHttpReadData(hRequest, buf + offset, bytesToRead, &len)) {
                    offset += len;
                }
                else {
                    break;
                }
            }
            buf[offset] = '\0';
            success = (offset > 0);
        }
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return success;
}