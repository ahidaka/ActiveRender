#pragma once

#include <Windows.h>
#include <stdio.h>
#include <AccCtrl.h>
#include <AclAPI.h>

//
class Registry {

public:
	//! Set REG_DWORD data
	HRESULT SetValue(__in PWSTR Path, __in PCWSTR Name, __in DWORD Value);

	//! Set REG_BINARY(DWORD) data
	HRESULT SetValue(__in PWSTR Path, __in PCWSTR Name, __in PDWORD Value, __in size_t Length);

	//! Set REG_SZ data
	HRESULT SetValue(__in PWSTR Path, __in PCWSTR Name, __in PWSTR Value);

	//! Get REG_DWORD data
	HRESULT GetValue(__in PWSTR Path, __in PCWSTR Name, __out PDWORD Value);

	//! Get REG_QWORD data
	HRESULT GetValue(__in PWSTR Path, __in PCWSTR Name, __out PULONG64 Value);

	//! Get REG_BINARY(DWORD) data
	HRESULT GetValue(__in PWSTR Path, __in PCWSTR Name, __out PBYTE pValue, __inout PDWORD pBufferLength);

	//! Get REG_SZ data
	HRESULT GetValue(__in PWSTR Path, __in PCWSTR Name, __out PWSTR pValue, __inout PDWORD pBufferLength);

	//!Writeble
	HRESULT SetKey(__in PWSTR Path, __in ACCESS_MASK Mask);

	//exists, ownership,
	HRESULT GetKey(__in PWSTR Path);

	HRESULT CreateKey(__in PWSTR Path, __in ACCESS_MASK Mask);

	HRESULT RemoveValue(__in PWSTR Path, __in DWORD Value);

	HRESULT RemoveKey(__in PWSTR Path, __in DWORD Value);

	//! Get CurrentUser's REG_SZ data
	HRESULT GetUserValue(__in PWSTR Path, __in PCWSTR Name, __out PWSTR pValue, __inout PDWORD pBufferLength);

private:
	//! Open key
	HKEY OpenKey(PWSTR Path, ACCESS_MASK Mask);

	//! Open key
	HKEY OpenKey(PWSTR Path, ACCESS_MASK Mask, HKEY RootKey);

	//! Open CurrentUser key
	HKEY OpenUserKey(PWSTR Path, ACCESS_MASK Mask);

	//! Change owner to me
	//HRESULT WriteOwner(PCWSTR Path);

	//! Change DAC to writable
	//HRESULT WriteDAC(PCWSTR Path);

	enum {
		DefaultBufferSize = MAX_PATH
	};

};
