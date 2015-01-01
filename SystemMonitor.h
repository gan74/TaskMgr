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
#include <QColor>
#include "PerfCounter.h"
#include "SystemUtils.h"

enum MonitorRole
{
	Cpu,
	Memory,
	Max
};

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

		double getMonitorValue(MonitorRole mon) const;

		double getTotalMemory() const;
		uint getCpuCount() const;

		static QColor getGraphColor(MonitorRole role) {
			static QColor colors[MonitorRole::Max] = {QColor(15, 125, 185), QColor(Qt::darkGreen)};
			return colors[role];
		}

		static double getGraphColorIntencity(double x) {
			x = pow(x, 0.8);
			x = 1.0 - x;
			x =  1.0 - x * x * x;
			return qMax(1.0 - x, 0.0) * 0.25 + 0.7;
		}

		static QString formatSize(double d) {
			QString s[] = {" B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
			int i = 0;
			for(; d >= 1024; d /= 1024, i++);
			d = round(d * 10) / 10;
			return QString::number(d, 'f', 1) + " " + s[i];
		}

	signals:
		void infoUpdated();

	protected:
		virtual void run() override;

	private:
		SystemMonitor();

		const double updateTime;

		double values[Max];

		double *coreValues;
		CpuPerfCounter **cpuCores;

		CpuPerfCounter cpuTotal;

		SystemInfo systemInfos;

		static SystemMonitor *monitor;
};

#endif // SYSTEMWATCHER_H
