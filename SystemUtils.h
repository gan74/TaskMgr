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

struct ProcessExtra
{
	double getCpuUsage(void *h);
	bool init = false;
	ullong last;
	ullong lastSys;
	ullong lastUser;
};

class ProcessDescriptor
{
	public:
		ProcessDescriptor() : exe(""), id(0), parent(0), prio(0) {
		}

		ProcessDescriptor(const ProcessDescriptor &p) : exe(p.exe), id(p.id), parent(p.parent), prio(p.prio) {
		}

		~ProcessDescriptor() {
		}

		QString exe;
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
	int cpus;
	double cpuUsage;
	double totalMemory;
	double usedMemory;
};


using ProcessList = QList<ProcessDescriptor>;

QString memString(double d);

bool enableDebugPrivileges(bool bEnable);
ProcessList getProcesses();
double getCpuUsage();
SystemInfo *getSystemInfo();


#endif // PROCESS_H
