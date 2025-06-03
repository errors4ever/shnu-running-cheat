#include "main.h"
#include "http.h"

BYTE pattern[] = { 0x3F, 0x7B, 0x22, 0x64, 0x61, 0x74, 0x61, 0x22, 0x3A, 0x22 };  // "?{"data":"
char userId[33] = { 0 };

char* fetchUserId() { //原登录过程使用微信内部标识符openId 只能通过内部api获取 所以无法模拟登录过程 选择读内存获取userId标识符
    HWND hwnd = FindWindowW(NULL, L"体锻打卡");
    if (!hwnd) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) return nullptr;
    uintptr_t addr = ScanProcessMemory(hProcess, pattern, sizeof(pattern));
    if (!addr) return nullptr;
    ReadProcessMemory(hProcess, (const void*)(addr + sizeof(pattern)), userId, 32, NULL);
    userId[32] = '\0';
    CloseHandle(hProcess);
    return userId;
}

int ExtractRunningRecord(const char* jsonResponse) { //懒得引入json库了 直接暴力匹配字符串找到runningRecord的值
    if (!jsonResponse) return -1;
    const char* target = "\"runningRecord\":";
    const char* pos = strstr(jsonResponse, target);
    if (pos) {
        pos += strlen(target);
        while (*pos == ' ' || *pos == '\t' || *pos == '"') pos++;
        return atoi(pos);
    }
    return -1;
}

bool CallOKAPI(const ok_Req& req, char* response, size_t responseSize) {
    std::string body = req.BuildBody();
    std::wstring headers = req.header;
    return SendHttpRequest(
        L"cpapp.1lesson.cn",
        L"/api/route/selectStudentSignIn",
        L"POST",
        headers.c_str(),
        body.c_str(),
        body.length(),
        response,
        responseSize,
        true
    );
}

bool CallStartRunAPI(const startRun_Req& req, char* response, size_t responseSize) {
    std::string body = req.BuildBody();
    std::wstring headers = req.header;
    return SendHttpRequest(
        L"cpapp.1lesson.cn",
        L"/api/route/insertStartRunning",
        L"POST",
        headers.c_str(),
        body.c_str(),
        body.length(),
        response,
        responseSize,
        true
    );
}

bool CallEndRunAPI(const endRun_Req& req, char* response, size_t responseSize) {
    std::string body = req.BuildBody();
    std::wstring headers = req.header;
    return SendHttpRequest(
        L"cpapp.1lesson.cn",
        L"/api/route/insertFinishRunning",
        L"POST",
        headers.c_str(),
        body.c_str(),
        body.length(),
        response,
        responseSize,
        true
    );
}

int main()
{
    char* userId = fetchUserId();
    char response[4096] = { 0 };
    startRun_Req startRunReq;
    startRunReq.userId = userId;
    CallStartRunAPI(startRunReq, response, sizeof(response));
    int recordId = ExtractRunningRecord(response);

    if (recordId != -1) {
        for (int i = 0; i < 4; ++i) {
            ok_Req okReq;
            okReq.userId = userId;
            okReq.recordId = recordId;
            okReq.posLongitude = Coords[i][0];
            okReq.posLatitude = Coords[i][1];
        }
        endRun_Req endRunReq;
        endRunReq.userId = userId;
        endRunReq.runningRecordId = recordId;
        CallEndRunAPI(endRunReq, response, sizeof(response));
    }
    return 0;
}