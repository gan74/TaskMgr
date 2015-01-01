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

#include "SystemMonitor.h"
#include <QMessageBox>

SystemMonitor *SystemMonitor::monitor = new SystemMonitor();

SystemMonitor::SystemMonitor() : QThread(), updateTime(0.5) {
	if(!enableDebugPrivileges(true)) {
		QMessageBox::warning(0, QObject::tr("Unable to get debug privileges."), QObject::tr("Unable to get debug privileges, did you start with administrator rights ?"));
	}
	coreValues = new double[systemInfos.cpus];
	cpuCores = new CpuPerfCounter*[systemInfos.cpus];
	for(uint i = 0; i != systemInfos.cpus; i++) {
		cpuCores[i] = new CpuPerfCounter(i);
		coreValues[i] = 0;
	}
	start();
}

SystemMonitor::~SystemMonitor() {
}

double SystemMonitor::getMemoryUsage() const {
	return values[MonitorRole::Memory];
}

double  SystemMonitor::getCpuUsage(int core) const {
	return core < 0 ? values[MonitorRole::Cpu] : coreValues[core];
}

double SystemMonitor::getMonitorValue(MonitorRole mon) const {
	return mon < 0 ? coreValues[-mon - 1] : values[mon];
}

double SystemMonitor::getTotalMemory() const {
	return systemInfos.totalMemory;
}

uint SystemMonitor::getCpuCount() const {
	return systemInfos.cpus;
}

void SystemMonitor::run() {
	qDebug("Monitor started");
	while(true) {
		values[MonitorRole::Cpu] = cpuTotal;
		values[MonitorRole::Memory] = getSystemMemoryUsage() * 100;
		for(uint i = 0; i != systemInfos.cpus; i++) {
			coreValues[i] = cpuCores[i]->getValue();
		}
		emit(infoUpdated());
		msleep(updateTime * 1000);
	}
}
