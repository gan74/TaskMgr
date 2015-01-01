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

#ifndef PROCESSMONITORWIDGET_H
#define PROCESSMONITORWIDGET_H

#include <QtWidgets>
#include "GraphView.h"
#include "ProcessMonitor.h"

class ProcessMonitorWidget : public QWidget
{
	Q_OBJECT
	public:
		explicit ProcessMonitorWidget(ProcessMonitor *mon, QWidget *parent = 0);
		~ProcessMonitorWidget();

	signals:

	public slots:
		void updateInfos();

	private:
		QWidget *createGraphView(MonitorRole role);
		ProcessMonitor *monitor;

		QLabel *reads;
		QLabel *writes;
};

#endif // PROCESSMONITORWIDGET_H
