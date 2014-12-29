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

#define NTDDI_WIN8
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <sstream>

#include <locale>

static_assert(sizeof(FILETIME) == sizeof(unsigned long long), "FILETIME should be the same as unsigned long long");

constexpr double cpuSmoothing = 0.5;

QString memString(double d) {
	QString s[] = {" B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	int i = 0;
	for(; d >= 1024; d /= 1024, i++);
	d = round(d * 10) / 10;
	return QString("%1 ").arg(d) + s[i];
}

ProcessList getProcesses() {
	QList<ProcessDescriptor> procs;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// Take a snapshot of all processes in the system.
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
		ProcessDescriptor info;
		std::wstring exe(pe32.szExeFile);
		info.exe = std::string(exe.begin(), exe.end()).c_str();
		DWORD dwPriorityClass = 0;
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if(hProcess == 0) {
			continue;
		} else {
			dwPriorityClass = GetPriorityClass(hProcess);
			if(!dwPriorityClass) {
				continue;
			}
			CloseHandle(hProcess);
		}

		info.id = pe32.th32ProcessID;
		info.parent = pe32.th32ParentProcessID;
		info.prio = dwPriorityClass;
		procs.append(info);
	} while(Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return procs;
}

HANDLE open(int id) {
	 return OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
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

uint ProcessDescriptor::getWorkingSet() const {
	HANDLE h = open(id);
	if(!h) {
		return 0;
	}
	PROCESS_MEMORY_COUNTERS pmc;
	if(!GetProcessMemoryInfo(h, &pmc, sizeof(pmc))) {
		return 0;
	}
	return pmc.WorkingSetSize;
}


double ProcessDescriptor::getCpuUsage() const {
	ullong now = 0;
	GetSystemTimeAsFileTime((LPFILETIME)&now);
	ullong n = 0;
	ullong sys = 0;
	ullong user = 0;
	HANDLE h = open(id);
	if(!h || !GetProcessTimes(h, (LPFILETIME)&n, (LPFILETIME)&n, (LPFILETIME)&sys, (LPFILETIME)&user)) {
		return -1;
	}
	if(!cpuTimes.last) {
		cpuTimes.last = now;
		cpuTimes.sys = sys;
		cpuTimes.user = user;
		return -1;
	}
	double cpu = user - cpuTimes.user + sys - cpuTimes.sys;
	ullong d = now - cpuTimes.last;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	if(!d || !sysInfo.dwNumberOfProcessors) {
		return -1;
	}
	cpu /= sysInfo.dwNumberOfProcessors;
	cpu /= d;
	cpuTimes.last = now;
	cpuTimes.sys = sys;
	cpuTimes.user = user;
	cpuTimes.acc = cpuTimes.acc * cpuSmoothing + cpu * (1 - cpuSmoothing);
	return cpuTimes.acc;
}

bool ProcessDescriptor::terminate() {
	HANDLE h = open(id);
	if(!h) {
		return false;
	}
	return TerminateProcess(h, 0);
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
	in->usedMemory = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	in->cpuUsage = -1;
	return in;
}

double cpuAcc = 0;
ullong lastTime = 0;
ullong lastIdle = 0;


double getCpuUsage() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	ullong idle = 0;
	ullong sys = 0;
	ullong user = 0;
	ullong now = 0;
	GetSystemTimeAsFileTime((LPFILETIME)&now);
	if(!GetSystemTimes((LPFILETIME)&idle, (LPFILETIME)&sys, (LPFILETIME)&user)) {
		return -1;
	}
	if(!lastTime) {
		lastTime =  now;
		lastIdle = idle;
		return -1;
	}
	ullong d = now - lastTime;
	ullong idleTime = idle - lastIdle;
	lastTime =  now;
	lastIdle = idle;
	double u = d ? 1 - (double(idleTime) / (double(d) * sysInfo.dwNumberOfProcessors)) : 0;
	cpuAcc = cpuAcc * cpuSmoothing + u * (1 - cpuSmoothing);
	return cpuAcc = u;
}
