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

	protected:
		virtual void paintEvent(QPaintEvent *event) override;

	private:
		struct Node
		{
			QTime time;
			double value;
		};

		double timeToDouble(const QTime &time) const;

		double timeWindow;
		double refreshRate;
		QList<Node> data;

		bool forceScroll;
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
		GraphView(const T &t, QWidget *parent = 0) : GraphViewBase(parent), updateTime(1), func(new Func<T>(t)) {
			updateGraph();
		}

		template<typename T>
		GraphView(double (*t)(), QWidget *parent = 0) : GraphViewBase(parent), updateTime(1), func(new FuncPtr(t)) {
			updateGraph();
		}

		~GraphView() {
			delete func;
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
		void updateGraph() {
			double c = func->apply();
			add(c);
			QTimer::singleShot(updateTime * 1000, this, updateGraph);
		}

		double updateTime;
		FuncBase *func;
};

#endif // GRAPHVIEW_H
