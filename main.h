#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "aobscan.h"
using namespace std;

//使用https://github.com/strengthen/AppletReverseTool逆向体锻打卡小程序 获取api接口以及相应请求数据结构

const double Coords[4][2] = {
    {121.51818147705078, 30.837721567871094},
    {121.52092847705076, 30.834883567871294},
    {121.51926147705322, 30.835872567871354},
    {121.51749847705033, 30.835306567871091},
};

struct ok_Req{
    std::wstring header = L"Content-Type: application/x-www-form-urlencoded\r\n";
    std::string userId;
    int recordId;
    double posLongitude;
    double posLatitude;

    std::string BuildBody() const {
        return "userId=" + userId +
            "&recordId=" + std::to_string(recordId) +
            "&posLongitude=" + std::to_string(posLongitude) +
            "&posLatitude=" + std::to_string(posLatitude);
    }
};

struct startRun_Req{
    std::wstring header = L"Content-Type: application/x-www-form-urlencoded\r\n";
    std::string userId;

    std::string BuildBody() const {
        return "userId=" + userId;
    }
};

struct endRun_Req{
    std::wstring header = L"Content-Type: application/json;charset=UTF-8\r\n";
    std::string userId;
    int runningRecordId;                //startRun的响应中字段recordId
    unsigned int mileage = 2;           //km
    unsigned int speedAllocation = 0;   //原小程序有一套计算方法 但应该不重要 或许可以抓包看看数据 填个随机数进去
    unsigned int totalTime = 1;         //25 - Math.floor(t / 60)
    unsigned int data = 0;              //data[0] = length

    std::string BuildBody() const {
        return "{\"userid\":\"" + userId +
            "\",\"runningRecordId\":" + std::to_string(runningRecordId) +
            ",\"mileage\":" + std::to_string(mileage) +
            ",\"speedAllocation\":" + std::to_string(speedAllocation) +
            ",\"totalTime\":" + std::to_string(totalTime) +
            ",\"data\":" + std::to_string(data) + "}";
    }
};

