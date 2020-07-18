
cl.exe /Zi /EHsc /Od /Fo:bin\ /Fd:bin\ /Fe:bin\Main.exe Main.cpp

if NOT EXIST bin (
	mkdir bin
)

if %ERRORLEVEL% NEQ 0 (
	pause
)