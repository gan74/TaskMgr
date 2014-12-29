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

SystemMonitor::SystemMonitor() : QThread(), updateTime(0.5), infos(getSystemInfo()) {
	if(!enableDebugPrivileges(true)) {
		QMessageBox::warning(0, QObject::tr("Unable to get debug privilege."), QObject::tr("Unable to get debug privilege, did you start with administrator rights ?"));
	}
	start();
}

SystemMonitor::~SystemMonitor() {
}

double SystemMonitor::getMemoryUsage() const {
	return memUsage;
}

double  SystemMonitor::getCpuUsage() const {
	return cpuUsage;
}

ullong SystemMonitor::getTotalMemory() const {
	return infos ? infos->totalMemory : 0;
}

uint SystemMonitor::getCpuCount() const {
	return infos ? infos->cpus : 0;
}

void SystemMonitor::run() {
	qDebug("Monitor started");
	while(true) {
		infos = infos ? infos : getSystemInfo();
		SystemPerformanceInfos perf = getSystemPerformanceInfos();
		cpuUsage = perf.cpu;
		memUsage = perf.mem;
		emit(infoUpdated());
		msleep(updateTime * 1000);
	}
}
