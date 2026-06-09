@echo off
cd /d "%~dp0..\gamemodes"
"%~dp0pawncc.exe" fg-ogc.pwn -;+ -(+ -i"%~dp0include" -i"maps" -i"systems" -r -w203 -d3
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Kompilacija uspjesna! fg-ogc.amx je spreman.
) else (
    echo.
    echo Kompilacija neuspjesna. Error level: %ERRORLEVEL%
)
pause
