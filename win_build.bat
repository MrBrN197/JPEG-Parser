
if NOT EXIST bin (
	mkdir bin
)

cl.exe /W3 /Zi /EHsc /Od /Fo:bin\ /Fd:bin\ /Fe:bin\Main.exe Main.cpp

if %ERRORLEVEL% NEQ 0 (
	pause
)