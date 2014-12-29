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

#include "PerformanceView.h"
#include "SystemMonitor.h"
#include <QtWidgets>

PerformanceView::PerformanceView(QWidget *parent) : QWidget(parent) {
	cpu = new GraphView([]() { return SystemMonitor::getMonitor()->getCpuUsage(); });
	mem = new GraphView([]() { return SystemMonitor::getMonitor()->getMemoryUsage(); });
	mem->setColor(Qt::green);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(cpu);
	layout->addWidget(mem);

	connect(SystemMonitor::getMonitor(), SIGNAL(infoUpdated()), cpu, SLOT(updateGraph()));
	connect(SystemMonitor::getMonitor(), SIGNAL(infoUpdated()), mem, SLOT(updateGraph()));
}

PerformanceView::~PerformanceView() {

}

