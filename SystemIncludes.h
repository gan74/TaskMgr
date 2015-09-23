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

#ifndef SYSTEMINCLUDES
#define SYSTEMINCLUDES

#define NTDDI_WIN8
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <sstream>

#include <locale>


typedef size_t uint;
typedef unsigned long long ullong;

constexpr double cpuSmoothing = 0.0;


#endif // SYSTEMINCLUDES

