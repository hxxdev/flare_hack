#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>
#include <iostream>
#include <psapi.h>
#include <string>

using namespace std;

void AssertError(string ErrorMessage) {
	cout << "[E] " + ErrorMessage << endl;
}

void AssertInfo(string InfoMessage) {
	cout << "[I] " + InfoMessage << endl;
}

HMODULE getModule(HANDLE processHandle) {
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModules(processHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		cout << "cb" << endl;
		cout << cbNeeded << endl;
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(processHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				wstring wstrModName = szModName;
				cout << "hi" << endl;
				wcout << szModName << endl;
				//you will need to change this to the name of the exe of the foreign process
				wstring wstrModContain = L"flare.exe";
				if (wstrModName.find(wstrModContain) != string::npos)
				{
					cout << "found" << endl;
					CloseHandle(processHandle);
					return hMods[i];
				}
			}
			else {
				DWORD ErrorCode = GetLastError();
				cout << ErrorCode << endl;
			}
		}
		CloseHandle(processHandle);
	}
	else {
		DWORD ErrorCode = GetLastError();
		LPWSTR message = NULL;
		ErrorCode = 0x5;
		//AssertError("EnumProcessModules Error Code :" + to_string(ErrorCode));
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR*)&message, 0, nullptr);
		printf("%s\n", message);
	}
	return nullptr;
}

int main(int argc, char** argv) {
	string ErrorMessage;

	// prefix L casts char* to const wchar_t*.
	HWND windowHandle = FindWindow(NULL, L"Flare");
	if (windowHandle == NULL) {
		AssertError("Window Flare not found");
		return 0;
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(windowHandle, &pid);
	AssertInfo("PID :" + to_string(pid));

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
	HMODULE module = getModule(processHandle);
	DWORD baseAddress = (DWORD)module;

	AssertInfo("Base Address :" + to_string(baseAddress));
	/*
	DWORD gold_address = 0;
	DWORD gold_value = 0;
	SIZE_T bytes_read = 0;
	ReadProcessMemory(process, (LPCVOID)(0x017EECB8 + 0x60), &gold_address, 4, &bytes_read);
	gold_address += 0xA90;
	ReadProcessMemory(process, (LPCVOID)gold_address, &gold_address, 4, &bytes_read);
	gold_address += 0x4;
	ReadProcessMemory(process, (LPCVOID)gold_address, &gold_value, 4, &bytes_read);

	DWORD new_gold_value = 9999;
	SIZE_T bytes_written = 0;
	WriteProcessMemory(process, (LPVOID)gold_address, &new_gold_value, 4, &bytes_written);
	*/
	return 0;
}