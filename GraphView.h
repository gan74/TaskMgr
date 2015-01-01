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

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "Graph.h"
#include <QWidget>
#include <QTime>
#include <QTimer>


class GraphView : public QWidget
{
	Q_OBJECT
	public:
		GraphView(QWidget *parent = 0);
		~GraphView();

		void setGraph(Graph *gr);

		void setColor(const QColor &c);
		void setViewport(double x1, double y1, double x2, double y2);
		void setGraduations(double x, double y);
		void setGraphHeightOffset(int off);

		static QColor defaultColor();

	protected:
		virtual void paintEvent(QPaintEvent *event) override;

	private:
		Graph *graph;

		QLineF viewport;
		QPointF grads;

		int graphHeightOffset;
		QColor color;
};
#endif // GRAPHVIEW_H
