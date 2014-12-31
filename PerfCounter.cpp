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

#include "PerfCounter.h"
#include "SystemIncludes.h"

class PdhPerfCounterImpl
{
	public:
		PdhPerfCounterImpl(const QString &name) {
			wchar_t *str = new wchar_t[name.size() + 1];
			name.toWCharArray(str);
			PdhOpenQuery(0, 0, &query);
			PdhAddCounter(query, str, 0, &counter);
			PdhCollectQueryData(query);
			delete str;
		}

		double getValue() {
			PDH_FMT_COUNTERVALUE val;
			PdhCollectQueryData(query);
			PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, 0, &val);
			return val.doubleValue;
		}

	private:
		PDH_HQUERY query;
		PDH_HCOUNTER counter;
};

PdhPerfCounter::PdhPerfCounter(const QString &name) : impl(new PdhPerfCounterImpl(name)){
}

PdhPerfCounter::~PdhPerfCounter() {
	delete impl;
}

double PdhPerfCounter::getValue() {
	return impl->getValue();
}


