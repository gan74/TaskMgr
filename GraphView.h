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

#include <QWidget>
#include <QTime>
#include <QTimer>


class GraphViewBase : public QWidget
{
	public:
		GraphViewBase(QWidget *parent = 0);
		~GraphViewBase();

		void add(double value);

		void setForceScrollEnabled(bool e);

		void setColor(const QColor &c);

	protected:
		virtual void paintEvent(QPaintEvent *event) override;

	private:
		struct Node
		{
			QDateTime time;
			double value;
		};

		double timeToDouble(const QDateTime &time) const;

		QDateTime start;

		double timeWindow;
		double refreshRate;
		QList<Node> data;

		bool forceScroll;
		bool backgroundScroll;
		QColor color;
};


class GraphView : public GraphViewBase
{
	Q_OBJECT

		struct FuncBase
		{
			virtual double apply() = 0;
			virtual ~FuncBase() {
			}
		};

		template<typename T>
		struct Func : public FuncBase
		{
			Func(const T &t) : f(t) {
			}

			virtual ~Func() {
			}

			virtual double apply() override {
				return f();
			}

			T f;
		};

		struct FuncPtr : public FuncBase
		{
				FuncPtr(double (*t)()) : f(t) {
				}

				virtual double apply() override {
					return f();
				}

				double (*f)();
		};

	public:
		template<typename T>
		GraphView(const T &t, QWidget *parent = 0) : GraphViewBase(parent), updateTime(0), func(new Func<T>(t)) {
		}

		template<typename T>
		GraphView(double (*t)(), QWidget *parent = 0) : GraphViewBase(parent), updateTime(0), func(new FuncPtr(t)) {
		}

		~GraphView() {
			delete func;
		}

		void setUpdateTime(double t) {
			updateTime = t;
			updateGraph();
		}

	public slots:
		void updateGraph() {
			double c = func->apply();
			add(c);
			if(updateTime) {
				QTimer::singleShot(updateTime * 1000, this, SLOT(updateGraph));
			}
		}

	protected:
		virtual void showEvent(QShowEvent *event) override {
			GraphViewBase::showEvent(event);
			updateTime /= 2;
		}

		virtual void hideEvent(QHideEvent *event) override {
			GraphViewBase::hideEvent(event);
			updateTime *= 2;
		}


	private:
		double updateTime;
		FuncBase *func;
};

#endif // GRAPHVIEW_H
