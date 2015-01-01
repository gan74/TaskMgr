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

	struct Node
	{
		double time;
		double value;
	};

	public:
		TimeGraph(QObject *parent = 0);

		void setTimeWindow(double t);
		double getTimeWindow() const;

		double add(double value);

		virtual QVector<QPointF> getPoints() const override;

	private:
		static QDateTime start;

		static double timeToDouble(const QDateTime &time = QDateTime::currentDateTime()) {
			return start.msecsTo(time) / 1000.0;
		}

		void removeOldest(double dt = timeToDouble());

		QList<Node> data;
		double timeWindow;
};

#endif // TIMEGRAPH

