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

#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include "ProcessDescriptor.h"
#include "TimeGraph.h"
#include "SystemMonitor.h"

class ProcessMonitorImpl;

class ProcessMonitor : public QObject
{
	Q_OBJECT

	public:
		ProcessMonitor(const ProcessDescriptor &d, QObject *parent = 0);
		virtual ~ProcessMonitor();

		uint getWorkingSet() const;
		double getCpuUsage() const;
		double getTotalCpuTime() const;

		uint getReads() const;
		uint getWrites() const;

		QDateTime getCreationTime() const;

		ProcessMonitor &operator=(const ProcessMonitor &) = delete;
		ProcessMonitor(const ProcessMonitor &) = delete;

		const ProcessDescriptor &getProcessDescriptor() const;

		TimeGraph *getGraph(MonitorRole mon);

	public slots:
		bool terminate();

	private:
		ProcessDescriptor proc;
		ProcessMonitorImpl *impl;

		TimeGraph *graphs[MonitorRole::Max];
};

#endif // PROCESSMONITOR_H
