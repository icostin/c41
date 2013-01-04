@echo off
:: [c41] make script for Microsoft Visual Studio 9
:: Changelog:
::  * 2013/01/04 Costin Ionescu: initial commit

set N=c41
set D=C41
set CSRC=src\fsi.c src\ilc.c src\io.c src\ma.c src\misc.c src\pmvops.c src\str.c  src\ucw8.c
set copts=/nologo /WX /W4
:: 4668 - warn when replacing undefined MACRO with 0
call %VS90COMNTOOLS%\vsvars32.bat

if not exist out\win32-rls-sl mkdir out\win32-rls-sl
if not exist out\win32-dbg-sl mkdir out\win32-dbg-sl
if not exist out\win32-rls-dl mkdir out\win32-rls-dl
if not exist out\win32-dbg-dl mkdir out\win32-dbg-dl

echo Building dynamic release library...  
cl.exe /Ox /LD %copts% /Iinclude /I..\c41\include /D%D%_DL_BUILD /DNDEBUG /Foout\win32-rls-dl\ /Feout\win32-rls-dl\%N%.dll %CSRC%
echo Building static release library...
cl.exe /Ox /c %copts% /Iinclude /I..\c41\include /D%D%_STATIC   /DNDEBUG /Foout\win32-rls-sl\ %CSRC%
set O=%CSRC:.c=.obj%
set O=%O:src=out\win32-rls-sl%
lib.exe /nologo /out:out\win32-rls-sl\%N%.lib %O%

echo Building dynamic debug library...  
cl.exe /Od /LDd %copts% /Iinclude /I..\c41\include /D%D%_DL_BUILD /D_DEBUG /Foout\win32-dbg-dl\ /Feout\win32-dbg-dl\%N%.dll %CSRC%
echo Building static debug library...
cl.exe /Od /c %copts% /Iinclude /I..\c41\include /D%D%_STATIC   /D_DEBUG /Foout\win32-dbg-sl\ %CSRC%
set O=%CSRC:.c=.obj%
set O=%O:src=out\win32-dbg-sl%
lib.exe /nologo /out:out\win32-dbg-sl\%N%.lib %O%

echo Building dynamic debug test program...
cl.exe /nologo /Od /Iinclude /I..\c41\include /D_DEBUG /Feout\win32-dbg-dl\test.exe /Foout\win32-dbg-dl\ src\test.c /link out\win32-dbg-dl\%N%.lib
out\win32-dbg-dl\test.exe
