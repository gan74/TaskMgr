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


#include "TimeGraph.h"

QDateTime TimeGraph::start = QDateTime::currentDateTime();

TimeGraph::TimeGraph() : timeWindow(30) {
}

void TimeGraph::setTimeWindow(double t) {
	timeWindow = t;
	removeOldest();
	emit(modified());
}

void TimeGraph::add(double value) {
	double dt = timeToDouble();
	data.append({dt, value});
	removeOldest(dt);
	emit(modified());
}

QVector<QPointF> TimeGraph::getPoints() const {
	double dt = timeToDouble();
	QVector<QPointF> pts;
	pts.reserve(data.size());
	for(const Node &n : data) {
		pts.append(QPointF(n.time - (dt - timeWindow), n.value));
	}
	return pts;
}

void TimeGraph::removeOldest(double dt) {
	if(timeWindow && !data.isEmpty()) {
		while(data.size() > 1 && dt - data[1].time > timeWindow * 1000) {
			data.removeFirst();
		}
		if(dt - data.first().time > timeWindow * 1000) {
			for(int i = 0; i < data.size() - 1; i++) {
				if(dt - data[i + 1].time > timeWindow * 1000) {
					for(; i > 0; i--) {
						data.removeFirst();
					}
				}
			}
		}
	}
}
