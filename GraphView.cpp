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

QColor GraphView::defaultColor() {
	return QColor(15, 125, 185);
}

GraphView::GraphView(QWidget *parent) : QWidget(parent), graph(0), viewport(0, 0, 1, 1), grads(0, 0), graphHeightOffset(5), color(defaultColor()) {
	setAutoFillBackground(true);
	setMinimumHeight(35);
}

GraphView::~GraphView() {
	setGraph(0);
}

void GraphView::setGraph(Graph *gr) {
	if(graph) {
		disconnect(graph, 0, this, 0);
		if(graph->parent() == this) {
			delete graph;
		}
	}
	graph = gr;
	if(graph) {
		if(!graph->parent()) {
			graph->setParent(this);
		}
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
	QColor gradColor = borderColor.lighter(230);
	QColor fillColor = QColor::fromHsl(color.hslHue(), color.hslSaturation(), 240);
	fillColor.setAlphaF(0.5);

	int graphMargin = 2;
	int heightOffset = graphHeightOffset;
	QRect clip(graphMargin, graphMargin, width() - 2 * graphMargin, height() - 2 * graphMargin);

	QLineF view = viewport;
	QVector<QPointF> points;
	if(graph) {
		points = graph->getPoints();
	}
	if(view.dy() <= 0 && !points.isEmpty()) {
		double min = points.first().y();
		double max = min;
		for(const QPointF &p : points) {
			min = qMin(min, p.y());
			max = qMax(max, p.y());
		}
		if(view.dy() < 0) {
			min = viewport.y1();
		} else {
			heightOffset = 0;
		}
		double d = (max - min) / 10;
		view = QLineF(view.x1(), min, view.x2(), max + d);
	}

	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
	QTransform trans;
	trans.translate(0, height());
	trans.scale(1, -1);
	trans.translate(graphMargin, graphMargin + heightOffset);
	trans.scale(clip.width() / view.dx(), view.dy() ? (clip.height() - 2 * heightOffset) / view.dy() : 1.0);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setClipRegion(clip);

	QPen gradPen(gradColor);
	gradPen.setWidthF(0.0);
	painter.setPen(gradPen);

	if(heightOffset) {
		painter.drawLine(QLineF(clip.x(), clip.y() + heightOffset, clip.x() + clip.width(), clip.y() + heightOffset));
		painter.drawLine(QLineF(clip.x(), clip.y() + clip.height() - heightOffset, clip.x() + clip.width(), clip.y() + clip.height() - heightOffset));
	}
	if(grads.x()) {
		for(int i = 0; i * grads.x() < view.dx(); i++) {
			double x = trans.map(QPointF(i * grads.x(), 0)).x();
			painter.drawLine(QLineF(x, clip.y() + heightOffset, x, clip.y() + clip.height() - heightOffset));
		}
	}
	if(view.dy()) {
		painter.setTransform(trans);
		if(grads.y()) {
			int i = 0;
			for(; i * grads.y() < view.dy(); i++) {
				painter.drawLine(QLineF(0, i * grads.y(), view.dx(), i * grads.y()));
			}
		}
		if(points.size() > 1) {
			QPolygonF poly(points);
			trans.translate(-view.x1(), -view.y1());
			painter.setTransform(trans);

			QPainterPath path;
			path.addPolygon(poly);
			path.lineTo(QPointF(points.last().x(), view.y1() - view.dy()));
			path.lineTo(QPointF(points.first().x(), view.y1() - view.dy()));
			painter.fillPath(path, QBrush(fillColor));

			QPen linePen(color);
			linePen.setWidthF(0.0);
			painter.setPen(linePen);
			painter.drawPolyline(poly);
		}
		painter.setTransform(QTransform());
	}
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(borderColor);
	painter.drawRect(clip.adjusted(0, 0, -1, -1));
}
