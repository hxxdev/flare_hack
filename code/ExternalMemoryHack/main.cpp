#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <iostream>
#include <psapi.h>
#include <fcntl.h>
#include <io.h>
#include <string>

using namespace std;

void AssertError(LPCWSTR ErrorMessage) {
	int msgboxID = MessageBox(
		NULL,
		ErrorMessage,
		(LPCWSTR)L"Error",
		MB_ICONWARNING | MB_OK | MB_DEFBUTTON1
	);
	switch (msgboxID) {
	case IDOK:
		// TODO: add code
		break;
	}
	return;
}

void HandleError() {
	DWORD ErrorCode = GetLastError();
	LPWSTR message = nullptr;

	if (ErrorCode == 0) {
		wprintf(L"No error to handle.\n");
		return;
	}

	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_ALLOCATE_BUFFER,
					   nullptr,
					   ErrorCode,
					   MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
					   (LPWSTR) &message,
					   0,
					   nullptr)) {
		wprintf(L"Format message failed with code 0x%x\n", GetLastError());
		return;
	}

	if (message) {
		wcout << message << endl;
		LocalFree(message);
	}
	return;
}

HMODULE getModule(HANDLE processHandle) {
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModulesEx(processHandle, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_64BIT))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(processHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				wstring wstrModName = szModName;
				//you will need to change this to the name of the exe of the foreign process
				wstring wstrModContain = L"flare.exe";
				if (wstrModName.find(wstrModContain) != string::npos)
				{
					return hMods[i];
				}
			}
			else {
				HandleError();
				return nullptr;
			}
		}
		// no module found
		wcout << L"no moudle found" << endl;
		CloseHandle(processHandle);
	}
	else {
		HandleError();
		return nullptr;
	}
	return nullptr;
}

int main(int argc, char** argv) {
	/* get window handle of Flare.exe. */
	HWND windowHandle = FindWindow(NULL, L"Flare");
	if (windowHandle == NULL) {
		HandleError();
		return -1;
	}
	wcout << "window found" << endl;

	/* get PID of Flare.exe. */
	DWORD pid = 0;
	if (!GetWindowThreadProcessId(windowHandle, &pid)) {
		HandleError();
		return -2;
	}
	wcout << L"PID :" + to_wstring(pid) << endl;

	/* get process handle of Flare.exe. */
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
	if (hProcess == NULL) {
		HandleError();
		return -3;
	}
	/* get base address of process Flare.exe. */
	HMODULE module = getModule(hProcess);
	DWORD64 BaseAddress = (DWORD64)module;
	wcout << hex << L"Base Address :" + to_wstring(BaseAddress) << endl;
	
	/* Adjust player position. */
	DWORD64 PlayerPosxAddress;
	float PlayerPosx;
	DWORD64 PlayerPosyAddress;
	float PlayerPosy;

	SIZE_T bytes_read = 0;

	ReadProcessMemory(hProcess, (LPCVOID)(BaseAddress + 0x00189750), &PlayerPosxAddress, sizeof(PlayerPosxAddress), &bytes_read);
	PlayerPosyAddress = PlayerPosxAddress;
	PlayerPosxAddress = PlayerPosxAddress + 0x3D0;
	ReadProcessMemory(hProcess, (LPCVOID)(PlayerPosxAddress), &PlayerPosx, sizeof(PlayerPosx), &bytes_read);
	PlayerPosyAddress = PlayerPosyAddress + 0x3CC;
	ReadProcessMemory(hProcess, (LPCVOID)(PlayerPosyAddress), &PlayerPosy, sizeof(PlayerPosy), &bytes_read);
	wcout << hex << to_wstring(PlayerPosx) << endl;
	wcout << hex << to_wstring(PlayerPosy) << endl;
	/*
	if (~ReadProcessMemory(hProcess, (LPCVOID)(0x22095EB3670), &PlayerPosxAddress, sizeof(PlayerPosxAddress), &bytes_read)) {
		HandleError();
		wcout << to_wstring(bytes_read) << endl;
		PlayerPosx = *(float*)&PlayerPosxAddress;
		wcout << hex << to_wstring(PlayerPosx) << endl;
		
		return -4;
	}*/

	//printf("bytes read : %d\n", bytes_read);
	//printf("Player posx Address : %ud\n", PlayerPosxAddress);
	//ReadProcessMemory(ProcessHandle, (LPCVOID)(PlayerPosxAddress + 0x3D0), & PlayerPosxAddress, 4, & bytes_read);
	//ReadProcessMemory(ProcessHandle, (LPCVOID)(PlayerPosxAddress), &PlayerPosx, 4, &bytes_read);
	//printf("Player posx : %f\n", PlayerPosx);

	//SIZE_T bytes_written = 0;
	//WriteProcessMemory(process, (LPVOID)gold_address, &new_gold_value, 4, &bytes_written);
	
	return 0;
}