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
#include "TimeGraph.h"
#include <QtWidgets>

PerformanceView::PerformanceView(QWidget *parent) : QWidget(parent), cpuGraph(new TimeGraph()), memGraph(new TimeGraph()), cpuView(new GraphView()), memView(new GraphView()) {
	cpuView->setGraduations(2, 0.25);
	cpuView->setGraph(cpuGraph);

	memView->setColor(Qt::darkGreen);
	memView->setGraduations(2, 0.25);
	memView->setGraph(memGraph);

	setGraphTimeWindow(30);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(cpuView);
	layout->addWidget(memView);

	connect(SystemMonitor::getMonitor(), SIGNAL(infoUpdated()), this, SLOT(updateGraphs()));
}

PerformanceView::~PerformanceView() {
}

void PerformanceView::setGraphTimeWindow(double t) {
	cpuView->setViewport(0, 0, t, 1);
	memView->setViewport(0, 0, t, 1);
	cpuGraph->setTimeWindow(t);
	memGraph->setTimeWindow(t);
}

void PerformanceView::updateGraphs() {
	cpuGraph->add(SystemMonitor::getMonitor()->getCpuUsage());
	memGraph->add(SystemMonitor::getMonitor()->getMemoryUsage());
}

