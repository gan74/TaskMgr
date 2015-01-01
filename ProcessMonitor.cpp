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

#include "ProcessMonitor.h"
#include "SystemIncludes.h"

class ProcessMonitorImpl
{
	public:
		ProcessMonitorImpl(const ProcessDescriptor &d) : handle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, d.id)) {
		}

		~ProcessMonitorImpl() {
			if(handle) {
				CloseHandle(handle);
			}
		}

		uint getWorkingSet() const {
			if(!handle) {
				return 0;
			}
			PROCESS_MEMORY_COUNTERS pmc;
			if(!GetProcessMemoryInfo(handle, &pmc, sizeof(pmc))) {
				return 0;
			}
			return pmc.WorkingSetSize;
		}


		double getCpuUsage() const {
			if(!handle) {
				return -1;
			}
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			ullong now = 0;
			ullong n = 0;
			ullong ker = 0;
			ullong user = 0;
			GetSystemTimeAsFileTime((LPFILETIME)&now);
			if(!GetProcessTimes(handle, (LPFILETIME)&n, (LPFILETIME)&n, (LPFILETIME)&ker, (LPFILETIME)&user)) {
				return -1;
			}
			if(!cpuTimes.lastTime) {
				cpuTimes.lastTime =  now;
				cpuTimes.lastKer = ker;
				cpuTimes.lastUser = user;
				return -1;
			}
			ullong dt = now - cpuTimes.lastTime;
			if(!dt) {
				return -1;
			}
			ullong userTime = user - cpuTimes.lastUser;
			ullong kerTime = ker - cpuTimes.lastKer;
			ullong sysTime = userTime + kerTime;
			cpuTimes.lastTime =  now;
			cpuTimes.lastKer = ker;
			cpuTimes.lastUser = user;
			double u = double(sysTime) / (double(dt) * sysInfo.dwNumberOfProcessors);
			return cpuTimes.acc = cpuTimes.acc * cpuSmoothing + u * (1.0 - cpuSmoothing);
		}

		bool terminate() {
			if(!handle) {
				return false;
			}
			return TerminateProcess(handle, 0);
		}

	private:
		mutable struct CpuTimes
		{
			CpuTimes() : lastTime(0), lastKer(0), lastUser(0), lastIdle(0), acc(0) {
			}

			ullong lastTime;
			ullong lastKer;
			ullong lastUser;
			ullong lastIdle;

			double acc;
		} cpuTimes;

		HANDLE handle;
};


ProcessMonitor::ProcessMonitor(const ProcessDescriptor &d) : impl(new ProcessMonitorImpl(d)), cpuGraph(new TimeGraph(this)), memGraph(new TimeGraph(this)) {
}

ProcessMonitor::~ProcessMonitor() {
	delete impl;
}

uint ProcessMonitor::getWorkingSet() const {
	return memGraph->add(impl->getWorkingSet());
}

double ProcessMonitor::getCpuUsage() const {
	double u = impl->getCpuUsage();
	if(u >= 0) {
		cpuGraph->add(u);
	}
	return u;
}

bool ProcessMonitor::terminate() {
	return impl->terminate();
}

TimeGraph *ProcessMonitor::getMemGraph() {
	return memGraph;
}

TimeGraph *ProcessMonitor::getCpuGraph() {
	return cpuGraph;
}
