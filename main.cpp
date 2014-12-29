/*******************************
Copyright (C) 2013-2014 gr�goire ANGERAND

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

#include "SystemUtils.h"
#include "TaskManager.h"
#include <iostream>
#include <QApplication>
#include <QMessageBox>

int main(int a, char **v) {
	QApplication app(a, v);
	TaskManager win;
	if(!enableDebugPrivileges(true)) {
		QMessageBox::warning(&win, QObject::tr("Unable to get debug privilege."), QObject::tr("Unable to get debug privilege, did you start with administrator rights ?"));
	}
	win.show();
	return app.exec();
}

