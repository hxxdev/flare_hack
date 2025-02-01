#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include "basic_tool.h"
#include "process_manager.h"

using namespace std;

int main(int argc, char** argv) {
	/* Open Process Flare.exe */
	LPCWSTR ProcessName = L"flare.exe";
	ProcessManager pManager;
	pManager.OpenProcessByProcessName(ProcessName);
	if (pManager.GetProcess() == NULL) {
		return 0;
	}

	/* get base address of process Flare.exe. */
	LPCWSTR ModuleName = L"flare.exe";
	DWORD64 BaseAddress = pManager.Get64BitBaseAddress(ModuleName);
	
	/* Adjust player position. */
	DWORD64 PlayerPosxAddress;
	DWORD64 PlayerPosyAddress;
	float PlayerPosx;
	float PlayerPosy;
	SIZE_T bytes_read = 0;
	
	HANDLE hProcess = pManager.GetProcess();
	while (1) {
		pManager.ReadProcess(BaseAddress + 0x00189750, &PlayerPosxAddress);
		PlayerPosyAddress = PlayerPosxAddress;

		/* Read player position x-axis coordinate */
		PlayerPosxAddress = PlayerPosxAddress + 0x3D0;
		pManager.ReadProcess(PlayerPosxAddress, &PlayerPosx);

		/* Read player position y-axis coordinate */
		PlayerPosyAddress = PlayerPosyAddress + 0x3CC;
		pManager.ReadProcess(PlayerPosyAddress, &PlayerPosy);
		
		wprintf(L"=======================\n");
		wprintf(L"player pos x :%f\n", PlayerPosx);
		wprintf(L"player pos y :%f\n", PlayerPosy);
		wprintf(L"=======================\n");

		Sleep(1000);
	}
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
	
	pManager.CloseProcess();

	return 0;
}