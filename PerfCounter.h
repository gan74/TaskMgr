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

#ifndef PERFCOUNTER_H
#define PERFCOUNTER_H

#include <QString>

class PdhPerfCounterImpl;

class PdhPerfCounter
{
	public:
		PdhPerfCounter(const QString &name);
		~PdhPerfCounter();

		double getValue();

		operator double() {
			return getValue();
		}

	private:
		PdhPerfCounterImpl *impl;
};

class CpuPerfCounter : public PdhPerfCounter
{
	public:
		CpuPerfCounter(int core = -1) : PdhPerfCounter(QString("\\Processor(%1)\\% Processor Time").arg(core < 0 ? "_Total" : QString::number(core))) {
		}
};

#endif // PERFCOUNTER_H
