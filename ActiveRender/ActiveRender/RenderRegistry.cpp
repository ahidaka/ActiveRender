#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <array>

#include "Registry.h"
#include "RenderRegistry.h"

#define LEVEL_DEBUG (0)

#define _LD if(LEVEL_DEBUG)

//
//
#define PERROR(code) RenderRegistry::ShowError(__func__, __LINE__, (code))

VOID RenderRegistry::ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode)
{
    CHAR buffer[MAX_PATH];
    DWORD dw = ErrorCode; // GetLastError();

    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        0,
        buffer,
        MAX_PATH,
        NULL);

    printf("%s(#%lu): Error(%d)=%s", Func, Line, dw, buffer);
}


VOID RenderRegistry::ActiveRender(PWSTR DeviceGuid)
{
    WCHAR parentKeyName[MAX_PATH];
    WCHAR subKeyName[MAX_PATH];
    WCHAR subKeyFullName[MAX_PATH];
    DWORD subKeyNameSize;
    FILETIME ftLastWriteTime;
    DWORD result = 0;
    HKEY pKey = NULL;
    PWSTR delimiter = (PWSTR)L"\\";
    DWORD valueLength = MAX_PATH;
    PWSTR  deviceState = (PWSTR)L"DeviceState";
    //PWSTR  level1 = (PWSTR)L"Level:0"; // Console
    PWSTR  level1 = (PWSTR)L"Level:1"; // Multimedia
    //PWSTR  level2 = (PWSTR)L"Level:2"; // Communication

    ULONG64 maxLevel = 0;
    //WCHAR maxDevice[MAX_PATH];

    Reg = new Registry();

    wcscpy_s(parentKeyName, MAX_PATH, MMDevices_Audio_Render);
    //        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Render");

    result = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        parentKeyName,
        0,
        KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY,
        &pKey);

    if (result != S_OK) {
        PERROR(result);
        return;
    }

    for (size_t dwIndex = 0; TRUE; dwIndex++) {

        DWORD dwResult;
        //DWORD type;
        DWORD dataSize = 0;
        //WCHAR data[MAX_PATH];
        DWORD data;
        ULONG64 data64;
        HKEY pPKey = NULL;

        subKeyNameSize = sizeof(subKeyName) / sizeof(subKeyName[0]);

        dwResult = RegEnumKeyEx(
            //HKEY_LOCAL_MACHINE,
            pKey,
            (DWORD)dwIndex,
            subKeyName,
            &subKeyNameSize,
            NULL,
            NULL,
            NULL,
            &ftLastWriteTime);

        if (dwResult == ERROR_SUCCESS) {

            //_tprintf(TEXT("SubKeyName: %s, LastWriteTime: %s\n"), subKeyName, szLastWriteTime);
            _LD _tprintf(TEXT("SubkeyName: %s\n"), subKeyName);

            wcscpy_s(subKeyFullName, MAX_PATH, parentKeyName);
            wcscat_s(subKeyFullName, MAX_PATH, delimiter);
            wcscat_s(subKeyFullName, MAX_PATH, subKeyName);

            result = Reg->GetValue(subKeyFullName, deviceState, &data);
            if (result != S_OK) {
                //PERROR(result);
                continue;
            }
            _LD printf("%ws=%lu(%lX)\n", deviceState, data, data);

            if (data != 1) {
                continue;
            }

            result = Reg->GetValue(subKeyFullName, level1, &data64);
            if (result != S_OK) {
                //PERROR(result);
                if (maxLevel == 0ULL) {
                    // default candidate
                    wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
                }
                continue;
            }
            _LD printf("%ws=%llu(%llX)\n", level1, data64, data64);

            if (DeviceGuid && (maxLevel <= data64)) {
                maxLevel = data64;
                wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
            }

#if 0
            result = Reg->GetValue(subKeyFullName, level1, &data64);
            if (result != S_OK) {
                PERROR(result);
                continue;
            }
            _LD printf("%ws=%llu(%llX)\n", level1, data64, data64);
#endif
        }
        else if (dwResult == ERROR_NO_MORE_ITEMS) {

            _LD puts("ERROR_NO_MORE_ITEMS");
            break;
        }
        else {

            _LD printf("RegEnumKeyEx error result=%08X(%d)\n", dwResult, dwResult);
            PERROR(dwResult);
            break;
        }
    }

    RegCloseKey(pKey);
}
