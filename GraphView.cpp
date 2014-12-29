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

GraphViewBase::GraphViewBase(QWidget *parent) : QWidget(parent), timeWindow(10), refreshRate(0.25), forceScroll(false), color(15, 125, 185) {
}

GraphViewBase::~GraphViewBase() {

}

void GraphViewBase::add(double value) {
	QTime time = QTime::currentTime();
	data.append({time, qMin(qMax(value, 0.0), 1.0)});
	while(data.size() > 1 && data[1].time.msecsTo(time) > timeWindow * 1000) {
		data.removeFirst();
	}
	if(data.first().time.msecsTo(time) > timeWindow * 1000) {
		for(int i = 0; i < data.size() - 1; i++) {
			if(data[i + 1].time.msecsTo(time) > timeWindow * 1000) {
				for(; i > 0; i--) {
					data.removeFirst();
				}
			}
		}
	}
	update();
}


void GraphViewBase::setColor(const QColor &c) {
	color = c;
}

void GraphViewBase::setForceScrollEnabled(bool e) {
	forceScroll = e;
	if(forceScroll) {
		update();
	}
}

double GraphViewBase::timeToDouble(const QTime &time) const {
	return 60 * time.minute() + time.second() + time.msec() / 1000.0;;
}

void GraphViewBase::paintEvent(QPaintEvent *event) {
	QTime time = forceScroll || data.isEmpty() ? QTime::currentTime() : data.last().time;
	double dTime = timeToDouble(time);

	QWidget::paintEvent(event);

	QColor base(15, 125, 185);
	QColor light = base.lighter(250);
	QColor back = QColor::fromHsl(color.hslHue(), color.hslSaturation(), 240);
	QPainter painter(this);
	int margins = 10;
	int dW = width() - 2 * margins;
	int dH = height() - 2 * margins;
	painter.setRenderHint(QPainter::Antialiasing, true);


	painter.setPen(light);
	double w = dW / timeWindow;
	double off = qMin(time.msec() / 1000.0, 1.0) * w;
	for(double i = 0; i < timeWindow; i++) {
		double x = dW - (off + i * w);
		painter.drawLine(QLineF(x + margins, margins, x + margins, dH + margins));
	}

	painter.setPen(color);
	painter.setClipping(true);
	painter.setClipRect(QRect(margins, margins, dW, dH));
	if(!data.isEmpty()) {
		QPointF last(0, 1);
		QPointF m(margins + 1, margins + 1);
		for(const Node &n : data) {
			double t = (timeToDouble(n.time) - (dTime - timeWindow)) / timeWindow;
			/*if(t < 0) {
				continue;
			}*/
			QPointF next(t * (dW - 4), (1.0 - n.value) * (dH - 4));
			/*if(next.x() - last.x() > 5)*/ {
				if(last.x() != 0) {
					QPolygonF fill(QVector<QPointF>{last + m,
													next + m,
													QPointF(next.x() + m.x() + 1, height() - margins),
													QPointF(last.x() + m.x(), height() - margins)});
					QPainterPath path;
					path.addPolygon(fill);
					painter.fillPath(path, QBrush(back));
					painter.drawLine(QLineF(last + m, next + m));
				}
				last = next;
			}
		}
	}
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(base);
	painter.drawRect(QRect(margins, margins, dW - 1, dH - 1));
	if(!forceScroll) {
		QTimer::singleShot(refreshRate * 1000, this, SLOT(update()));
	}
}
