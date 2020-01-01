@echo off

rem
rem DRNSF - An unofficial Crash Bandicoot level editor
rem Copyright (C) 2017-2020  DRNSF contributors
rem
rem See the AUTHORS.md file for more details.
rem
rem This program is free software: you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program.  If not, see <http://www.gnu.org/licenses/>.
rem

pushd "%~dp0"

echo Drag and drop a BIN file onto this batch file to apply the region:
echo   SCEA (NTSC-U/C - North America)
echo:

if "%1"=="" goto no_args

set /p "ok=The file(s) will be overwritten. OK? (y/n)"
if "%ok%"=="y" goto ok
if "%ok%"=="Y" goto ok
if "%ok%"=="yes" goto ok
if "%ok%"=="YES" goto ok
if "%ok%"=="Yes" goto ok
echo Files will not be processed.
goto exit

:ok
echo Processing files...
..\drnsf.exe :cdxa-imprint --psx-scea %*
echo Done.
goto exit

:no_args
echo No files were given.
goto exit

:exit
echo:
pause

popd
