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
	processView = new ProcessView();
	processView->setUniformRowHeights(true);
	processView->setMinimumSize(400, 400);
	processView->resize(400, 600);

	terminateButton = new QPushButton(tr("End task"));
	terminateButton->setEnabled(false);


	QWidget *main = new QWidget();
	QVBoxLayout *mainLayout = new QVBoxLayout(main);
	QHBoxLayout *bottomLayout = new QHBoxLayout();

	mainLayout->addWidget(processView);
	mainLayout->addLayout(bottomLayout);
	bottomLayout->addStretch();
	bottomLayout->addWidget(terminateButton);
	addTab(main, tr("Processes"));

	addTab(new PerformanceView(), tr("Performance"));

	QTimer *viewUpdateTimer = new QTimer(this);
	viewUpdateTimer->setInterval(processViewUpdateTime);
	QTimer *selectionUpdateTimer = new QTimer(this);
	selectionUpdateTimer->setInterval(processSelectionUpdateTime);

	connect(processView, QTreeWidget::itemSelectionChanged, this, TaskManager::processSelected);
	connect(terminateButton, QPushButton::clicked, this, TaskManager::terminate);
	connect(processView, SIGNAL(viewUpdated()), viewUpdateTimer, SLOT(start()));
	connect(viewUpdateTimer, QTimer::timeout, processView, ProcessView::populateView);
	selectionUpdateTimer->start();
	processView->populateView();

	//setUp(this);
}

TaskManager::~TaskManager() {
}

void TaskManager::processSelected() {
	terminateButton->setEnabled(true);
}

void TaskManager::terminate() {
	if(processView->selectedItems().isEmpty()) {
		return;
	}
	dynamic_cast<ProcessView::Item *>(processView->selectedItems().first())->terminateProcess();
}

