#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <array>

#include "Registry.h"
#include "EndpointRegistry.h"

//#define LEVEL_DEBUG (1)
//#define _LD if(LEVEL_DEBUG)
#define _LD if(debug)

#define PERROR(code) EndpointRegistry::ShowError(__func__, __LINE__, (code))

VOID EndpointRegistry::ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode)
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

//
// This function is controlled by those keywords.
//	 Keywords flow;
//   Keywords role;
//
VOID EndpointRegistry::ActiveEndpoint(PWSTR DeviceGuid)
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
    PWSTR  level0 = (PWSTR)L"Level:0"; // Console
    PWSTR  level1 = (PWSTR)L"Level:1"; // Multimedia
    PWSTR  level2 = (PWSTR)L"Level:2"; // Communication

    ULONG64 maxLevelmm = 0;
    ULONG64 maxLevelunkown = 0;
    ULONG64 maxLevelcom = 0;

    Reg = new Registry();

    if (DeviceGuid == NULL)
    {
        printf("ActiveEndpoint: DeviceGuid is NULL\n");
        return;
    }

    //    wcscpy_s(parentKeyName, MAX_PATH, MMDevices_Audio_Render);
    //        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Render");

    if (flow == Keywords::Render) {
		wcscpy_s(parentKeyName, MAX_PATH, MMDevices_Audio_Render);
	}
    else if (flow == Keywords::Capture) {
		wcscpy_s(parentKeyName, MAX_PATH, MMDevices_Audio_Capture);
	}
	else {
        // error
        printf("ActiveEndpoint: flow error=%d\n", flow);
		return;
	}

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
        ULONG64 data64mm;
        ULONG64 data64unknown;
        ULONG64 data64com;
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

            //
            // level1:assume the default candidate for Multimedia role
            //
            if (role == Keywords::Multimedia) {
                result = Reg->GetValue(subKeyFullName, level1, &data64mm);
                if (result != S_OK) {
                    if (maxLevelmm == 0ULL) {
                        // default candidate
                        wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
                    }
                }
                if (DeviceGuid && (maxLevelmm <= data64mm)) {
                    maxLevelmm = data64mm;
                    wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
                }

                _LD printf("mm: %ws=%llu(%llX)\n", level1, data64mm, data64mm);
            }

            //
            // level1: This level id for Unknown role
            //
            if (role == Keywords::Unknown) {
				result = Reg->GetValue(subKeyFullName, level1, &data64unknown);
				if (result != S_OK) {
					if (maxLevelunkown == 0ULL) {
						wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
					}
				}
				if (DeviceGuid && (maxLevelunkown <= data64unknown)) {
					maxLevelunkown = data64unknown;
					wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
				}

				_LD printf("uk: %ws=%llu(%llX)\n", level1, data64unknown, data64unknown);
			}

            //
            // level2: This level id for Communication role
            //
            if (role == Keywords::Communication) {
                result = Reg->GetValue(subKeyFullName, level1, &data64com);
                if (result != S_OK) {
                    if (maxLevelcom == 0ULL) {
                        wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
                    }
                }
                if (DeviceGuid && (maxLevelcom <= data64com)) {
                    maxLevelcom = data64com;
                    wcscpy_s(DeviceGuid, MAX_PATH, subKeyName);
                }

                _LD printf("co: %ws=%llu(%llX)\n", level2, data64com, data64com);
            }
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
