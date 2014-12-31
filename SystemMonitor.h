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

#ifndef SYSTEMWATCHER_H
#define SYSTEMWATCHER_H

#include <QThread>

class SystemInfo;
class MemoryPerfCounter;
class CpuPerfCounter;

class SystemMonitor : public QThread
{
	Q_OBJECT

	public:
		~SystemMonitor();

		static SystemMonitor *getMonitor() {
			return monitor;
		}

		double getMemoryUsage() const;
		double getCpuUsage(int core = -1) const;

		double getTotalMemory() const;
		uint getCpuCount() const;

	signals:
		void infoUpdated();

	protected:
		virtual void run() override;

	private:
		SystemMonitor();

		const double updateTime;

		struct
		{
			double mem;
			double cpuTotal;
			double *cpuCores;
		} perfInfos;

		struct
		{
			MemoryPerfCounter *mem;
			CpuPerfCounter *cpuTotal;
			CpuPerfCounter **cpuCores;
		} counters;


		SystemInfo *retrieveSystemInfos();
		SystemInfo *systemInfos;

		static SystemMonitor *monitor;
};

#endif // SYSTEMWATCHER_H
