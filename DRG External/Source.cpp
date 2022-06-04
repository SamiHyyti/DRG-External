#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include "tchar.h"

using namespace std;

HANDLE hProc = NULL;
DWORD proc_id = NULL;
HWND hwnd;

uintptr_t getModuleBase(DWORD pID, const char* modName) {
	uintptr_t modBaseAdd = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_tcscmp(modEntry.szModule, modName))
				{
					modBaseAdd = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAdd;
}

void getpos(float x, float y, float z, bool c) {
	cout << x << endl;
	cout << y << endl;
	cout << z << endl;
	if (c)
		cout << "Dance toggle: ON\n";
	else
		cout << "Dance toggle: OFF\n";
}

uintptr_t readOffsets(uintptr_t clientBase, vector<uintptr_t> offs) {
	uintptr_t address = clientBase;
	for (int i = 0; i < offs.size() - 1; i++) {
		ReadProcessMemory(hProc, (LPVOID)(address + offs.at(i)), &address, sizeof(address), NULL);
	}
	return address += offs.at(offs.size() - 1);
}

int main() {
	vector<uintptr_t> xaxis = { 0x03743A30 ,0x30 ,0x338 ,0x4E8 ,0x158 ,0xF0 ,0x108, 0x1A0 };
	vector<uintptr_t> yaxis = { 0x03743A30 ,0x30 ,0x338 ,0x4E8 ,0x158 ,0xF0 ,0x108, 0x1A4 };
	vector<uintptr_t> zaxis = { 0x03743A30 ,0x30 ,0x338 ,0x4E8 ,0x158 ,0xF0 ,0x108, 0x1A8 };
	vector<uintptr_t> dance = { 0x03743A30 ,0x30 ,0x148 ,0x10 ,0x440 ,0x278 ,0x248, 0xE8A };

	uintptr_t clientBase = NULL;

	char moduleName[] = "FSD-Win64-Shipping.exe";

	hwnd = FindWindow(NULL, "Deep Rock Galactic  ");
	if (!hwnd) {
		cout << "Window not found!\n";
		return 1;
	}
	GetWindowThreadProcessId(hwnd, &proc_id);
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
	if (!hProc) {
		cout << "Process not found!\n";
		return 2;
	}
	clientBase = getModuleBase(proc_id, moduleName);

	bool runscript = false;
	float x, y, z;
	float newx, newy, newz;
	bool onoff = false;
	while (!GetAsyncKeyState(VK_DELETE))
	{
		ReadProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, xaxis)), &x, sizeof(x), NULL);
		ReadProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, yaxis)), &y, sizeof(x), NULL);
		ReadProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, zaxis)), &z, sizeof(x), NULL);
		ReadProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, dance)), &onoff, sizeof(onoff), NULL);
		getpos(x, y, z, onoff);
		if (GetAsyncKeyState(VK_NUMPAD1)) {
			newx = 65;
			newy = -35;
			newz = -360;
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, xaxis)), &newx, sizeof(newx), NULL);
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, yaxis)), &newy, sizeof(newy), NULL);
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, zaxis)), &newz, sizeof(newz), NULL);
		}
		if (GetAsyncKeyState(VK_NUMPAD2)) {
			newx = 2350;
			newy = -100;
			newz = -630;
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, xaxis)), &newx, sizeof(newx), NULL);
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, yaxis)), &newy, sizeof(newy), NULL);
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, zaxis)), &newz, sizeof(newz), NULL);
		}
		if (GetAsyncKeyState(VK_NUMPAD5)) {
			onoff = !onoff;
			WriteProcessMemory(hProc, (LPVOID)(readOffsets(clientBase, dance)), &onoff, sizeof(bool), NULL);
		}

		Sleep(10);
		system("cls");
	}

	return 0;
}