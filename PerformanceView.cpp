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
#include <iostream>

PerformanceView::PerformanceView(QWidget *parent) : QWidget(parent), cpuGraph(new TimeGraph()), coreGraphs(0), memGraph(new TimeGraph()), cpuView(new GraphView()), coreViews(0), memView(new GraphView()) {
	cpuView->setGraduations(2, 0.25);
	cpuView->setGraph(cpuGraph);

	memView->setColor(Qt::darkGreen);
	memView->setGraduations(2, 0.25);
	memView->setGraph(memGraph);

	QVBoxLayout *layout = new QVBoxLayout(this);
	coreLayout = new QGridLayout();
	layout->addWidget(cpuView);
	layout->addLayout(coreLayout);
	layout->addWidget(memView);

	createCores();
	setGraphTimeWindow(30);

	connect(SystemMonitor::getMonitor(), SIGNAL(infoUpdated()), this, SLOT(updateGraphs()));
}

PerformanceView::~PerformanceView() {
}

void PerformanceView::createCores() {
	if(coreGraphs) {
		return;
	}
	uint cpus = SystemMonitor::getMonitor()->getCpuCount();
	if(cpus) {
		uint rows = cpus > 4 && cpus % 2 == 0 ? 2 : 1;
		coreGraphs = new TimeGraph*[cpus];
		coreViews = new GraphView*[cpus];
		for(uint i = 0; i != cpus; i++) {
			coreViews[i] = new GraphView();
			coreGraphs[i] = new TimeGraph();
			coreViews[i]->setGraduations(2, 0.25);
			coreViews[i]->setGraph(coreGraphs[i]);
			coreLayout->addWidget(coreViews[i], i % rows, i / rows);
		}
	}
}

void PerformanceView::setGraphTimeWindow(double t) {
	cpuView->setViewport(0, 0, t, 1);
	memView->setViewport(0, 0, t, 1);
	cpuGraph->setTimeWindow(t);
	memGraph->setTimeWindow(t);

	createCores();
	if(coreGraphs) {
		for(uint i = 0; i != SystemMonitor::getMonitor()->getCpuCount(); i++) {
			coreViews[i]->setViewport(0, 0, t, 1);
			coreGraphs[i]->setTimeWindow(t);
		}
	}
}

void PerformanceView::updateGraphs() {
	cpuGraph->add(SystemMonitor::getMonitor()->getCpuUsage());
	memGraph->add(SystemMonitor::getMonitor()->getMemoryUsage());
	if(coreGraphs) {
		for(uint i = 0; i != SystemMonitor::getMonitor()->getCpuCount(); i++) {
			coreGraphs[i]->add(SystemMonitor::getMonitor()->getCpuUsage(i));
		}
	}
}

