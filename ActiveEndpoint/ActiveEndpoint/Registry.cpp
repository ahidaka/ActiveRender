#include <Windows.h>
#include <stdio.h>
#include "Registry.h"

VOID ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode)
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

  //  _PRINT(_DBG_ERROR, "%s(#%lu): Error(%d)=%s", Func, Line, dw, buffer);
}

#pragma warning(push)
#pragma warning(disable:4267)
#pragma warning(disable:6101)
#pragma warning(disable:6054)
#pragma warning(disable:28183)

//! Open key
HKEY Registry::OpenKey(PWSTR Path, ACCESS_MASK Mask, HKEY RootKey)
{
    HKEY rootKey = RootKey;
    HKEY accessKey;
    HRESULT result;
    REGSAM  samDesired = KEY_QUERY_VALUE | KEY_WOW64_64KEY;
    
    if (Mask & GENERIC_WRITE) {
        samDesired |= KEY_SET_VALUE;
    }
    if (Mask & WRITE_OWNER) {
        samDesired |= WRITE_OWNER;
    }
    if (Mask & WRITE_DAC) {
        samDesired |= READ_CONTROL | WRITE_DAC;
    }
    result = RegOpenKeyExW(rootKey, Path, 0, samDesired, &accessKey);
    RegCloseKey(rootKey);
    return result == S_OK ? accessKey : 0;
}

//! Open key
HKEY Registry::OpenKey(PWSTR Path, ACCESS_MASK Mask)
{
    return OpenKey(Path, Mask, HKEY_LOCAL_MACHINE);
}

//! Open key
HKEY Registry::OpenUserKey(PWSTR Path, ACCESS_MASK Mask)
{
    return OpenKey(Path, Mask, HKEY_CURRENT_USER);
}

//! Set REG_DWORD data
HRESULT Registry::SetValue(__in PWSTR Path, __in PCWSTR Name, __in DWORD Value)
{
    HRESULT result;
    HKEY key = OpenKey(Path, GENERIC_WRITE);

    result = RegSetValueExW(key, Name, 0, REG_DWORD, (const BYTE*) &Value, sizeof(DWORD));
    RegCloseKey(key);
    if (result != S_OK) {
        // Change access privilege and owner
        result = SetKey(Path, WRITE_DAC | WRITE_OWNER);
        if (result == S_OK) {
            result = RegSetValueExW(key, Name, 0, REG_DWORD, (const BYTE*)&Value, sizeof(DWORD));
        }
    }
    return result;
}

//! Set REG_BINARY(DWORD) data
HRESULT Registry::SetValue(__in PWSTR Path, __in PCWSTR Name, __in PDWORD Value, __in size_t Length)
{
    HRESULT result;
    HKEY key = OpenKey(Path, GENERIC_WRITE);

    result = RegSetValueExW(key, Name, 0, REG_BINARY, (const BYTE*)&Value, Length * sizeof(DWORD));
    RegCloseKey(key);
    if (result != S_OK) {
        // Change access privilege and owner
        result = SetKey(Path, WRITE_DAC | WRITE_OWNER);
        if (result == S_OK) {
            result = RegSetValueExW(key, Name, 0, REG_BINARY, (const BYTE*)&Value, Length * sizeof(DWORD));
        }
    }
    return result;
}

//! Set REG_SZ data
HRESULT Registry::SetValue(__in PWSTR Path, __in PCWSTR Name, __in PWSTR Value)
{
    HRESULT result;
    HKEY key = OpenKey(Path, GENERIC_WRITE);

    result = RegSetValueExW(key, Name, 0, REG_SZ, (const BYTE*)&Value, wcslen(Value));
    RegCloseKey(key);
    if (result != S_OK) {
        // Change access privilege and owner
        result = SetKey(Path, WRITE_DAC | WRITE_OWNER);
        if (result == S_OK) {
            result = RegSetValueExW(key, Name, 0, REG_SZ, (const BYTE*)&Value, wcslen(Value));
        }
    }
    return result;
}

//! Get REG_DWORD data
HRESULT Registry::GetValue(__in PWSTR Path, __in PCWSTR Name, __out PDWORD Value)
{
	HRESULT result = S_OK;
	DWORD type = 0;
    DWORD buffer[DefaultBufferSize / sizeof(DWORD)];
	DWORD dataLength = DefaultBufferSize / sizeof(DWORD);
    HKEY key = OpenKey(Path, GENERIC_READ);

	result = RegQueryValueExW(key, Name, NULL, &type, (LPBYTE) buffer, &dataLength);
	if (result != S_OK) {
        RegCloseKey(key);
        return result;
	}
    if (type != REG_DWORD || Value == NULL) {
        result = ERROR_INVALID_PARAMETER;
    }
    else {
        *Value = buffer[0];
    }
    RegCloseKey(key);
    return result;
}

//! Get REG_QWORD data
HRESULT Registry::GetValue(__in PWSTR Path, __in PCWSTR Name, __out PULONG64 Value)
{
    HRESULT result = S_OK;
    DWORD type = 0;
    ULONG64 buffer[DefaultBufferSize / sizeof(ULONG64)];
    DWORD dataLength = DefaultBufferSize / sizeof(ULONG64);
    HKEY key = OpenKey(Path, GENERIC_READ);

    result = RegQueryValueExW(key, Name, NULL, &type, (LPBYTE)buffer, &dataLength);
    if (result != S_OK) {
        RegCloseKey(key);
        return result;
    }
    if (type != REG_QWORD || Value == NULL) {
        result = ERROR_INVALID_PARAMETER;
    }
    else {
        *Value = buffer[0];
    }
    RegCloseKey(key);
    return result;
}

//! Get REG_BINARY(DWORD) data
HRESULT Registry::GetValue(__in PWSTR Path, __in PCWSTR Name, __out PBYTE pValue, __inout PDWORD pBufferLength)
{
    HRESULT result = S_OK;
    DWORD type = 0;
    DWORD dataLength = 0;
    HKEY key = OpenKey(Path, GENERIC_READ);

    result = RegQueryValueExW(key, Name, NULL, &type, NULL, &dataLength);
    if (result != S_OK) {
        return result;
    }
    if (type != REG_SZ || pValue == NULL || pBufferLength == NULL
        || (*pBufferLength * sizeof(DWORD)) < dataLength) {
        RegCloseKey(key);
        result = ERROR_INVALID_PARAMETER;
        return result;
    }

    result = RegQueryValueExW(key, Name, NULL, &type, (LPBYTE)pValue, &dataLength);
    RegCloseKey(key);
    if (result != S_OK) {
        return result;
    }
    *pBufferLength = dataLength / sizeof(DWORD);
    return result;
}

//! Get REG_SZ data
HRESULT Registry::GetValue(__in PWSTR Path, __in PCWSTR Name, __out PWSTR pValue, __inout PDWORD pBufferLength)
{
    HRESULT result = S_OK;
    DWORD type = 0;
    DWORD dataLength = 0;
    HKEY key = OpenKey(Path, GENERIC_READ);

    result = RegQueryValueExW(key, Name, NULL, &type, NULL, &dataLength);
    if (result != S_OK) {
        return result;
    }
    if ((type != REG_SZ && type != REG_EXPAND_SZ)
        || pValue == NULL || pBufferLength == NULL || *pBufferLength < dataLength) {
        RegCloseKey(key);
        result = ERROR_INVALID_PARAMETER;
        return result;
    }

    result = RegQueryValueExW(key, Name, NULL, &type, (LPBYTE) pValue, &dataLength);
    RegCloseKey(key);
    if (result != S_OK) {
        return result;
    }
    *pBufferLength = dataLength;
    return result;
}

//! Get Get CurrentUser's REG_SZ data
HRESULT Registry::GetUserValue(__in PWSTR Path, __in PCWSTR Name, __out PWSTR pValue, __inout PDWORD pBufferLength)
{
    HRESULT result = S_OK;
    DWORD type = 0;
    DWORD dataLength = 0;
    HKEY key = OpenUserKey(Path, GENERIC_READ);

    result = RegQueryValueExW(key, Name, NULL, &type, NULL, &dataLength);
    if (result != S_OK) {
        RegCloseKey(key);
        return result;
    }

    if (type != REG_SZ || pValue == NULL || pBufferLength == NULL || *pBufferLength < dataLength) {
        RegCloseKey(key);
        result = ERROR_INVALID_PARAMETER;
        return result;
    }

    result = RegQueryValueExW(key, Name, NULL, &type, (LPBYTE)pValue, &dataLength);
    RegCloseKey(key);
    if (result != S_OK) {
        return result;
    }
    *pBufferLength = dataLength;
    return result;
}

//Writeble
HRESULT Registry::SetKey(__in PWSTR Path, __in ACCESS_MASK Mask)
{
    UNREFERENCED_PARAMETER(Path);
    HRESULT result = S_OK;

    if (Mask & WRITE_OWNER) {
        //** Todo Simethig to set
        //result = WriteOwner(Path);
        if (result != S_OK) {
            return result;
        }
    }
    if (Mask & WRITE_DAC) {
        //** Todo Simethig to set
        //result = WriteDAC(Path);
        if (result != S_OK) {
            return result;
        }
    }
    return result;
}

//exists, ownership,
HRESULT Registry::GetKey(__in PWSTR Path)
{
    //HRESULT result = S_OK;
    HKEY key = OpenKey(Path, GENERIC_WRITE);
    RegCloseKey(key);
    return(key != NULL ? S_OK : ERROR_NOT_FOUND);
}

HRESULT Registry::CreateKey(__in PWSTR Path, __in ACCESS_MASK Mask)
{
    HRESULT result = S_OK;
    HKEY rootKey = HKEY_LOCAL_MACHINE;
    HKEY accessKey;
    REGSAM  samDesired = KEY_QUERY_VALUE | KEY_WOW64_64KEY;

    if (Mask & GENERIC_WRITE) {
        samDesired |= KEY_SET_VALUE;
    }
    if (Mask & WRITE_OWNER) {
        samDesired |= WRITE_OWNER;
    }
    if (Mask & WRITE_DAC) {
        samDesired |= READ_CONTROL | WRITE_DAC;
    }
    result = RegCreateKeyExW(rootKey, Path, 0, NULL, 0, samDesired, NULL, &accessKey, NULL);
    RegCloseKey(rootKey);
    RegCloseKey(rootKey);

    return result;
}

HRESULT Registry::RemoveValue(__in PWSTR Path, __in DWORD Value)
{
    UNREFERENCED_PARAMETER(Path);
    UNREFERENCED_PARAMETER(Value);
    HRESULT result = S_OK;
    return result;
}

HRESULT Registry::RemoveKey(__in PWSTR Path, __in DWORD Value)
{
    UNREFERENCED_PARAMETER(Path);
    UNREFERENCED_PARAMETER(Value);
    HRESULT result = S_OK;
    return result;
}

#pragma warning(pop)
