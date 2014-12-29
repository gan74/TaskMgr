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

#ifndef PROCESSVIEW
#define PROCESSVIEW

#include <QtGui>
#include <QtWidgets>
#include "SystemUtils.h"

class ProcessView : public QTreeWidget
{
	Q_OBJECT
	public:
		enum Columns
		{
			Name = 0,
			PID = 1,
			Parent = 2,
			CPU = 3,
			WorkingSet = 4,
			Max
		};

		class Item : public QTreeWidgetItem
		{
			public:
				Item(ProcessView *view, const ProcessDescriptor &p);
				const ProcessDescriptor &getProcessDescriptor() const;

				void updatePerformanceInfos();
				bool terminateProcess();

				bool operator<(const QTreeWidgetItem &i) const {
					int col = treeWidget()->sortColumn();
					const Item *it = dynamic_cast<const Item *>(&i);
					if(col == CPU) {
						return cpuUsage < it->cpuUsage;
					}
					if(col == PID) {
						return proc.id < it->proc.id;
					}
					if(col == Parent) {
						return proc.parent < it->proc.parent;
					}
					if(col == WorkingSet) {
						return workingSet < it->workingSet;
					}
					return QTreeWidgetItem::operator<(i);
				}

			private:
				void setProcessDescriptor(const ProcessDescriptor &p);
				void updateBackground();
				ProcessDescriptor proc;
				double cpuUsage;
				uint workingSet;
		};

		class ItemDelegate : public QItemDelegate
		{
			public:
				ItemDelegate() {
				}

				QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const {
					return QSize(600, 23);
				}
		};

		ProcessView(QWidget *parent = 0);
		virtual ~ProcessView();

	public slots:
		void populateView();
		void updateSystemInfos();
		void updateSelection();

	signals:
		void viewUpdated();

	private:
		QStringList getHeaderLabels();
		Item *findItem(ProcessDescriptor d) const;
};

#endif // PROCESSVIEW

