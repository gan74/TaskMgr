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

#ifndef PROCESS_H
#define PROCESS_H

#include <cinttypes>
#include <QtCore>

typedef size_t uint;
typedef unsigned long long ullong;

class ProcessDescriptor
{
	public:
		~ProcessDescriptor() {
		}

		QString exe;
		QString name;
		int id;
		int parent;
		int prio;

		bool operator==(const ProcessDescriptor &p) const {
			return id == p.id;
		}

		uint getWorkingSet() const;
		double getCpuUsage() const;
		bool terminate();

		mutable struct CpuTime
		{
			CpuTime() : last(0), acc(0) {
			}

			ullong last;
			ullong sys;
			ullong user;

			double acc;
		} cpuTimes;
};

struct SystemInfo
{
	uint cpus;
	ullong totalMemory;
};

struct SystemPerformanceInfos
{
	double cpu;
	double mem;
};


using ProcessList = QList<ProcessDescriptor>;

QString memString(double d);

bool enableDebugPrivileges(bool bEnable);
ProcessList getProcesses();

SystemInfo *getSystemInfo();
SystemPerformanceInfos getSystemPerformanceInfos();


#endif // PROCESS_H
