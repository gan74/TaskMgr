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

#include "ProcessMonitorWidget.h"
#include "SystemMonitor.h"
#include <QtWidgets>

ProcessMonitorWidget::ProcessMonitorWidget(ProcessMonitor *mon, QWidget *parent) : QWidget(parent), monitor(mon) {
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(createGraphView(MonitorRole::Cpu));
	layout->addWidget(createGraphView(MonitorRole::Memory));
}

ProcessMonitorWidget::~ProcessMonitorWidget() {
	//qDebug("~ProcessMonitorWidget()");
}

QWidget *ProcessMonitorWidget::createGraphView(MonitorRole role) {
	GraphView *view = new GraphView();
	view->setGraph(monitor->getGraph(role));
	view->setAllowCompactMode(false);
	view->setColor(SystemMonitor::getGraphColor(role));
	view->setGraduations(1, 0);
	view->setViewport(0, 0, monitor->getGraph(role)->getTimeWindow(), -1);
	return view;
}
