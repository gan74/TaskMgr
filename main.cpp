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

class Busyhread : public QThread
{
	public:
		Busyhread() : QThread() {
		}

		virtual void run() override {
			std::cout<<"Busy thread started !"<<std::endl;
			while(true) {
				for(int i = 0; i > -1; i++) {
				}
				sleep(10);
			}
		}
};

int main(int a, char **v) {
	QApplication app(a, v);
	TaskManager win;
	win.show();
	//(new Busyhread())->start();
	return app.exec();
}

