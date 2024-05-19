#pragma once

#include <Windows.h>
#include "Registry.h"

//VOID ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode);
//VOID GetKeyRegistory(PCSTR Mode);
//VOID RegistoryStatus();

class EndpointRegistry {

public:
	enum class Keywords :int
	{
		Capture = 0,
		Render = 1,
		Multimedia = 2,
		Communication = 3,
		Unknown = -1,
		DEFAULT_FLOW = Render,
		DEFAULT_ROLE = Multimedia
	};

	Keywords flow = EndpointRegistry::Keywords::DEFAULT_FLOW;
	Keywords role = EndpointRegistry::Keywords::DEFAULT_ROLE;
	int debug = 0;

	PCWSTR MMDevices_Audio_Render =
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Render";
	PCWSTR MMDevices_Audio_Capture =
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Capture";

	PCWSTR _Propaties = L"Properties";
	PCWSTR _FxPropaties = L"FxProperties";
	
	static VOID ShowError(PCSTR Func, DWORD Line, DWORD ErrorCode);

	///VOID AnalizeKeyEntry(PWSTR SubKey);

	//VOID GetKeyRegistory(PCSTR Mode);

	VOID ActiveEndpoint(PWSTR DeviceGuid);

private:
	Registry* Reg = NULL;
};
