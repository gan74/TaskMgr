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

#include "ProcessView.h"
#include "SystemMonitor.h"
#include <iostream>

ProcessView::Item::Item(ProcessView *view, const ProcessDescriptor &p) : QTreeWidgetItem(view), cpuUsage (-1), workingSet(0) {
	setProcessDescriptor(p);
	setTextAlignment(Name, Qt::AlignLeft | Qt::AlignVCenter);
	for(int i = 1; i < Max; i++) {
		setTextAlignment(i, Qt::AlignRight | Qt::AlignVCenter);
	}
}

const ProcessDescriptor &ProcessView::Item::getProcessDescriptor() const {
	return proc;
}

void ProcessView::Item::setProcessDescriptor(const ProcessDescriptor &p) {
	proc = p;
	setText(Name, proc.name);
	setText(PID, QString::number(proc.id));
	setText(Parent, QString::number(proc.parent));
}

bool ProcessView::Item::terminateProcess() {
	bool r = proc.terminate();
	if(r) {
		setFlags(flags() & ~Qt::ItemIsEnabled);
	}
	return r;
}

void ProcessView::Item::updatePerformanceInfos() {
	setText(WorkingSet, memString(workingSet = proc.getWorkingSet()));
	double cpu = proc.getCpuUsage();
	cpuUsage = cpu < 0 || cpu > 1 ? -1 : cpu;
	setText(CPU, cpuUsage < 0 ? "" : QString::number(round(cpu * 1000) / 10) + "%");
	updateBackground();
}

double curve(double x) {
	x = pow(x, 0.8);
	x = 1.0 - x;
	return 1.0 - x * x * x;
}

void ProcessView::Item::updateBackground() {

	QColor bg(Qt::white);
	double hue = 0.56;
	double sat = 0.78;
	double baseL = 0.7;
	double p = 0.4;
	double L = 0.25;
	if(cpuUsage >= 0) {
		bg.setHslF(hue, sat, pow(1.0 - qMin(cpuUsage, 1.0), p) * L + baseL);
	}
	setBackgroundColor(CPU, bg);
	bg.setHslF(hue, sat, qMax(1.0 - curve(qMin(workingSet / (512.0 * 1024 * 1024), 1.0)), 0.0) * L + baseL);
	setBackgroundColor(WorkingSet, bg);
}

QStringList ProcessView::getHeaderLabels() {
	return {QObject::tr("Name"),
			QObject::tr("PID"),
			QObject::tr("Parent"),
			QObject::tr("CPU"),
			QObject::tr("Memory") + QString(" (%1%)").arg(round(SystemMonitor::getMonitor()->getMemoryUsage() * 100))};
}

ProcessView::ProcessView(QWidget *parent) : QTreeWidget(parent) {
	setHeaderLabels(getHeaderLabels());
	//setSelectionMode(QAbstractItemView::SingleSelection);
	header()->setSectionHidden(Parent, true);
	setSortingEnabled(true);
	sortByColumn(Name, Qt::AscendingOrder);
	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(QHeaderView::Fixed);
	header()->setSectionResizeMode(Name, QHeaderView::Stretch);
	setItemDelegate(new ItemDelegate());
}

ProcessView::~ProcessView() {
}

void ProcessView::populateView() {
	ProcessList infos = getProcesses();
	for(int i = 0; i != invisibleRootItem()->childCount(); i++) {
		invisibleRootItem()->child(i)->setHidden(true);
	}
	for(ProcessDescriptor i : infos) {
		Item *item = findItem(i);
		if(!item) {
			item = new Item(this, i);
		} else {
			item->setHidden(false);
		}
		item->updatePerformanceInfos();
	}
	for(int i = 0; i < invisibleRootItem()->childCount(); i++) {
		if(invisibleRootItem()->child(i)->isHidden()) {
			delete invisibleRootItem()->takeChild(i--);
		}
	}
	updateSystemInfos();
	emit(viewUpdated());
}

void ProcessView::updateSelection() {
	if(!selectedItems().isEmpty()) {
		for(QTreeWidgetItem *i : selectedItems()) {
			Item *item = dynamic_cast<Item *>(i);
			item->updatePerformanceInfos();
		}
	}
}

void ProcessView::updateSystemInfos() {
	setHeaderLabels(getHeaderLabels());
}

ProcessView::Item *ProcessView::findItem(ProcessDescriptor d) const {
	for(int i = 0; i != invisibleRootItem()->childCount(); i++) {
		Item *item = dynamic_cast<Item *>(invisibleRootItem()->child(i));
		if(item->getProcessDescriptor() == d) {
			return item;
		}
	}
	return 0;
}

