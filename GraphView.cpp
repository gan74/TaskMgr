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

#include "GraphView.h"
#include <QtGui>
#include <iostream>

GraphView::GraphView(QWidget *parent) : QWidget(parent), graph(0), viewport(0, 0, 1, 1), grads(0, 0), graphHeightOffset(5), color(15, 125, 185) {
	setMinimumHeight(35);
}

GraphView::~GraphView() {
}

void GraphView::setGraph(Graph *gr) {
	if(graph) {
		disconnect(graph, 0, this, 0);
	}
	graph = gr;
	if(graph) {
		graph->setParent(this);
		connect(graph, SIGNAL(modified()), this, SLOT(update()));
	}
	update();
}

void GraphView::setColor(const QColor &c) {
	color = c;
	update();
}

void GraphView::setViewport(double x1, double y1, double x2, double y2) {
	viewport = QLineF(x1, y1, x2, y2);
	update();
}

void GraphView::setGraduations(double x, double y) {
	grads = QPointF(x, y);
	update();
}

void GraphView::setGraphHeightOffset(int off) {
	graphHeightOffset = off;
	update();
}

void GraphView::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
	QColor borderColor(15, 125, 185);
	QColor gradColor = borderColor.lighter(237);
	QColor fillColor = QColor::fromHsl(color.hslHue(), color.hslSaturation(), 240);
	fillColor.setAlphaF(0.5);

	int graphMargin = 2;
	QRect clip(graphMargin, graphMargin, width() - 2 * graphMargin, height() - 2 * graphMargin);

	QTransform trans;
	trans.translate(0, height());
	trans.scale(1, -1);
	trans.translate(graphMargin, graphMargin + graphHeightOffset);
	trans.scale(clip.width() / viewport.dx(), (clip.height() - 2 * graphHeightOffset) / viewport.dy());

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setClipRegion(clip);
	painter.setTransform(trans);

	QPen gradPen(gradColor);
	gradPen.setWidthF(0.0);

	painter.setPen(gradPen);
	if(grads.x()) {
		for(int i = 0; i * grads.x() < viewport.dx(); i++) {
			painter.drawLine(QLineF(i * grads.x(), 0, i * grads.x(), viewport.dy()));
		}
	}
	if(grads.y()) {
		int i = 0;
		for(; i * grads.y() < viewport.dy(); i++) {
			painter.drawLine(QLineF(0, i * grads.y(), viewport.dx(), i * grads.y()));
		}
		painter.drawLine(QLineF(0, i * grads.y(), viewport.dx(), i * grads.y()));
	}

	if(graph) {
		QPolygonF poly(graph->getPoints() << QPointF((viewport.x1() + viewport.x2()) * 0.5, -1e15));
		if(poly.size() > 3) {
			trans.translate(-viewport.x1(), -viewport.y1());
			painter.setTransform(trans);

			QPainterPath path;
			path.addPolygon(poly);
			painter.fillPath(path, QBrush(fillColor));

			QPen linePen(color);
			linePen.setWidthF(0.0);
			painter.setPen(linePen);
			painter.drawPolyline(poly);
		}
	}

	painter.setTransform(QTransform());
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(borderColor);
	painter.drawRect(clip.adjusted(0, 0, -1, -1));
}
