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

#ifndef PROCESSDESCRIPTOR_H
#define PROCESSDESCRIPTOR_H

#include <QString>

class ProcessDescriptor
{
	public:
		enum Priority
		{
			Idle = 0,
			Low = 1,
			Normal = 2,
			High = 3,
			VeryHigh = 4,
			RealTime = 5,
			Max
		};


		static QString getPriorityName(Priority p) {
			QString names[] = {"Idle", "Low", "Normal", "High", "Very hight", "Real time", "Unknown"};
			return names[p];
		}


		ProcessDescriptor(const QString &e, const QString &n, int i, int par, Priority pri) : exe(e), name(n), id(i), parent(par), prio(pri) {
		}

		ProcessDescriptor() : ProcessDescriptor("", "", -1, -1, Max) {
		}

		ProcessDescriptor(const ProcessDescriptor &) = default;

		QString exe;
		QString name;
		int id;
		int parent;
		Priority prio;

		QString getPriorityName() const {
			return getPriorityName(prio);
		}

		bool operator==(const ProcessDescriptor &p) const {
			return id == p.id;
		}
};


#endif // PROCESSDESCRIPTOR_H
