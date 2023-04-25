#pragma once

#include <Windows.h>
#include "Registry.h"

//VOID ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode);
//VOID GetKeyRegistory(PCSTR Mode);
//VOID RegistoryStatus();

class RenderRegistry {

public:
	PCWSTR MMDevices_Audio_Render =
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Render";
	PCWSTR _Propaties = L"Properties";
	PCWSTR _FxPropaties = L"FxProperties";
	
	static VOID ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode);

	///VOID AnalizeKeyEntry(PWSTR SubKey);

	//VOID GetKeyRegistory(PCSTR Mode);

	VOID ActiveRender(PWSTR DeviceGuid);

private:
	Registry* Reg = NULL;
};
