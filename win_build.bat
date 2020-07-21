
if NOT EXIST bin (
	mkdir bin
)

cl.exe /Zi /EHsc /Od /Fo:bin\ /Fd:bin\ /Fe:bin\Main.exe Main.cpp

if %ERRORLEVEL% NEQ 0 (
	pause
)