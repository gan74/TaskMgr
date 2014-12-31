/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/

#include "SystemUtils.h"
#include "SystemIncludes.h"

static_assert(sizeof(FILETIME) == sizeof(unsigned long long), "FILETIME should be the same as unsigned long long");

QList<ProcessDescriptor> getProcesses() {
	QList<ProcessDescriptor> procs;
	wchar_t buffer[512] = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE) {
		return procs;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnap, &pe32)) {
		CloseHandle(hProcessSnap);
		return procs;
	}
	do {
		DWORD dwPriorityClass = 0;
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		SetLastError(0);
		if(hProcess == 0) {
			continue;
		} else {
			dwPriorityClass = GetPriorityClass(hProcess);
			if(!dwPriorityClass) {
				continue;
			}
		}
		buffer[GetProcessImageFileName(hProcess, buffer, sizeof(buffer) / sizeof(wchar_t))] = 0;

		std::wstring exe(buffer);
		std::wstring name(pe32.szExeFile);
		CloseHandle(hProcess);
		if(GetLastError()) {
			SetLastError(0);
			continue;
		}
		procs.append(ProcessDescriptor(std::string(exe.begin(), exe.end()).c_str(),
									   std::string(name.begin(), name.end()).c_str(),
									   pe32.th32ProcessID,
									   pe32.th32ParentProcessID,
									   dwPriorityClass));
	} while(Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return procs;
}

HANDLE open(const ProcessDescriptor &d) {
	 return OpenProcess(PROCESS_ALL_ACCESS, FALSE, d.id);
}

bool enableDebugPrivileges(bool bEnable) {
	HANDLE hToken = 0;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		return false;
	}

	LUID luid;
	if(!LookupPrivilegeValue(0, SE_DEBUG_NAME, &luid)) {
		return false;
	}

	TOKEN_PRIVILEGES tokenPriv;
	tokenPriv.PrivilegeCount = 1;
	tokenPriv.Privileges[0].Luid = luid;
	tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	if(!AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), 0, 0)) {
		return false;
	}

	return true;
}



SystemInfo *getSystemInfo() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if(!GlobalMemoryStatusEx(&memInfo)) {
		qDebug("Unable to get system informations.");
		return 0;
	}
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	SystemInfo *in = new SystemInfo();
	in->cpus = sysInfo.dwNumberOfProcessors;
	in->totalMemory = memInfo.ullTotalPageFile;
	return in;
}

double getSystemMemoryUsage() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if(!GlobalMemoryStatusEx(&memInfo)) {
		return -1;
	}
	return double(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile) / memInfo.ullTotalPageFile;
}

double getSystemCpuUsage() {
	static double cpuAcc = 0;
	static ullong lastTime = 0;
	static ullong lastUser = 0;
	static ullong lastKer = 0;
	static ullong lastIdle = 0;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	ullong idle = 0;
	ullong ker = 0;
	ullong user = 0;
	ullong now = 0;
	GetSystemTimeAsFileTime((LPFILETIME)&now);
	if(!GetSystemTimes((LPFILETIME)&idle, (LPFILETIME)&ker, (LPFILETIME)&user)) {
		return -1;
	}
	if(!lastTime) {
		lastTime =  now;
		lastKer = ker;
		lastUser = user;
		lastIdle = idle;
		return -1;
	}
	ullong dt = now - lastTime;
	if(!dt) {
		return -1;
	}
	ullong idleTime = idle - lastIdle;
	ullong userTime = user - lastUser;
	ullong kerTime = ker - lastKer;
	ullong sysTime = userTime + kerTime;
	lastTime =  now;
	lastKer = ker;
	lastUser = user;
	lastIdle = idle;
	double u = double(sysTime - idleTime) / (double(sysTime) * sysInfo.dwNumberOfProcessors);
	return cpuAcc = cpuAcc * cpuSmoothing + u * (1.0 - cpuSmoothing);
}
