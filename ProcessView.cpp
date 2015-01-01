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
#include "SystemUtils.h"

ProcessView::Item::Item(ProcessView *view, const ProcessDescriptor &p) : QTreeWidgetItem(view), proc(p), mon(new ProcessMonitor(proc)), cpuUsage(-1), workingSet(0) {
	setText(Name, proc.name);
	setText(PID, QString::number(proc.id));
	setText(Parent, QString::number(proc.parent));
	setTextAlignment(Name, Qt::AlignLeft | Qt::AlignVCenter);
	for(int i = 1; i < Max; i++) {
		setTextAlignment(i, Qt::AlignRight | Qt::AlignVCenter);
	}
}

ProcessView::Item::~Item() {
	delete mon;
}

const ProcessDescriptor &ProcessView::Item::getProcessDescriptor() const {
	return proc;
}

ProcessMonitor *ProcessView::Item::getProcessMonitor() {
	return mon;
}

void ProcessView::Item::updatePerformanceInfos() {
	setText(WorkingSet, SystemMonitor::formatSize(workingSet = mon->getWorkingSet()));
	double cpu = mon->getCpuUsage();
	cpuUsage = cpu < 0 || cpu > 1 ? -1 : cpu;
	setText(CPU, cpuUsage < 0 ? "" : QString::number(cpu, 'f', 1) + "%");
	updateBackground();
}

void ProcessView::Item::updateBackground() {
	QColor bg(Qt::white);
	double hue = 0.56;
	double sat = 0.78;
	if(cpuUsage >= 0) {
		bg.setHslF(hue, sat, SystemMonitor::getGraphColorIntencity(cpuUsage / SystemMonitor::getMonitor()->getCpuCount()));
	}
	setBackgroundColor(CPU, bg);
	bg.setHslF(hue, sat, SystemMonitor::getGraphColorIntencity(workingSet / (512.0 * 1024 * 1024)));
	setBackgroundColor(WorkingSet, bg);
}

QStringList ProcessView::getHeaderLabels() {
	return {QObject::tr("Name"),
			QObject::tr("PID"),
			QObject::tr("Parent"),
			QObject::tr("CPU") + QString(" (%1%)").arg(round(SystemMonitor::getMonitor()->getCpuUsage())),
			QObject::tr("Memory") + QString(" (%1%)").arg(round(SystemMonitor::getMonitor()->getMemoryUsage()))};
}

ProcessView::ProcessView(QWidget *parent) : QTreeWidget(parent), monitor(0) {
	setHeaderLabels(getHeaderLabels());
	//setIndentation(0);
	setTreePosition(Max);
	setExpandsOnDoubleClick(false);
	setSelectionMode(QAbstractItemView::SingleSelection);
	header()->setSectionHidden(Parent, true);
	setSortingEnabled(true);
	sortByColumn(Name, Qt::AscendingOrder);
	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(QHeaderView::Fixed);
	header()->setSectionResizeMode(Name, QHeaderView::Stretch);
	setItemDelegate(new ItemDelegate(this));
	updateTimer.setSingleShot(true);
	updateTimer.setInterval(1000);
	connect(&updateTimer, SIGNAL(timeout()), this, SLOT(populateView()));
	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)), this, SLOT(openMonitor(QTreeWidgetItem*)));
}

ProcessView::~ProcessView() {
}

void ProcessView::showEvent(QShowEvent *event) {
	QTreeWidget::showEvent(event);
	populateView();
}

void ProcessView::populateView() {
	QList<ProcessDescriptor> infos = getProcesses();
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
	if(isVisible()) {
		updateTimer.start();
	}
	emit(viewUpdated());
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

void ProcessView::openMonitor(QTreeWidgetItem *item) {
	Item *it = dynamic_cast<Item *>(item);
	if(monitor) {
		monitor->parent()->setSelected(false);
		monitor->parent()->removeChild(monitor);
		delete monitor;
		monitor = 0;
	}
	if(it) {
		monitor = new MonitorItem(it);
		item->setExpanded(true);
		item->setSelected(true);
		monitor->setSizeHint(0, QSize(600, 100));
	}
}

