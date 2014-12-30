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

#ifndef TIMEGRAPH
#define TIMEGRAPH

#include "Graph.h"
#include <QObject>
#include <QDateTime>

class TimeGraph : public Graph
{
	Q_OBJECT
	public:
		TimeGraph() : start(QDateTime::currentDateTime()), timeWindow(30) {
		}

		void add(double value) {
			QDateTime time = QDateTime::currentDateTime();
			double dt = timeToDouble(time);
			data.append({dt, value});
			if(timeWindow) {
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
			emit(modified());
		}

		virtual QVector<QPointF> getPoints() const override {
			double dTime = timeToDouble(QDateTime::currentDateTime());
			QVector<QPointF> pts;
			pts.reserve(data.size());
			for(const Node &n : data) {
				pts.append(QPointF(n.time - (dTime - timeWindow), n.value));
			}
			return pts;
		}

	private:
		struct Node
		{
			double time;
			double value;
		};

		double timeToDouble(const QDateTime &time) const {
			return start.msecsTo(time) / 1000.0;
		}

		QDateTime start;
		QList<Node> data;

		double timeWindow;
};

#endif // TIMEGRAPH

