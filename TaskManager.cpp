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

#include "TaskManager.h"
#include "PerformanceView.h"

#include <windows.h>
typedef struct {
	int cxLeftWidth;
	int cxRightWidth;
	int cyTopHeight;
	int cyBottomHeight;
} MARGINS;
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *margins);
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL *pfEnabled);
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;

void setUp(QWidget *wid) {
	if(!wid->parent()) {
		if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {
			QLibrary *winapi = new QLibrary("dwmapi");
			pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)winapi->resolve("DwmIsCompositionEnabled");
			BOOL enabled = FALSE;
			pDwmIsCompositionEnabled(&enabled);
			if(enabled) {
				pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)winapi->resolve("DwmExtendFrameIntoClientArea");
				MARGINS margins = {-1, -1, -1, -1};
				auto id = wid->winId();
				pDwmExtendFrameIntoClientArea(*(HWND *)&id, &margins);
				wid->setAutoFillBackground(false);
				wid->setAttribute(Qt::WA_TranslucentBackground);
			}
		}
	}
}

TaskManager::TaskManager(QWidget *parent) : QTabWidget(parent) {
	addTab(new ProcessView(), tr("Processes"));
	addTab(new PerformanceView(), tr("Performance"));
	resize(800, 600);

	//setUp(this);
}

TaskManager::~TaskManager() {
}


