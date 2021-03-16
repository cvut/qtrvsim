@echo off

rem lib/build.bat - build script for W32 port
rem Copyright (C) 2004 - 2006 Michael Riepe
rem
rem This library is free software; you can redistribute it and/or
rem modify it under the terms of the GNU Library General Public
rem License as published by the Free Software Foundation; either
rem version 2 of the License, or (at your option) any later version.
rem
rem This library is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
rem Library General Public License for more details.
rem
rem You should have received a copy of the GNU Library General Public
rem License along with this library; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

rem @(#) $Id: build.bat,v 1.1 2006/08/21 18:03:48 michael Exp $

rem *** BEGIN EDIT HERE ***
rem Please uncomment the line that suits your system:
rem call "C:\Program Files\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
rem call "C:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
rem call "C:\Programme\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
rem call "C:\Programme\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
rem OR, if you have to set the path to the compiler directly:
rem set PATH="C:\PATH\TO\COMPILER\BINARY;%PATH%"
rem Of course, you'll have to enter the correct path above.
rem You may also have to change CC (default: cl.exe) in Makefile.w32.
rem *** END EDIT HERE ***

copy config.h.w32 config.h
copy sys_elf.h.w32 sys_elf.h
nmake /nologo /f Makefile.w32 %1
