@echo off
title Unicate Gaming Launcher - Compile to EXE
color 0A
echo ============================================
echo    UNICATE GAMING - Launcher Compiler
echo ============================================
echo.
echo Ova skripta ce kompajlirati launcher.py u .exe fajl
echo tako da igraci ne trebaju imati Python instaliran.
echo.
echo Provjera PyInstaller-a...
echo.

pip show pyinstaller >nul 2>&1
if %errorlevel% neq 0 (
    echo PyInstaller nije instaliran. Instaliram...
    pip install pyinstaller
    echo.
)

echo Kompajliranje launchera u .exe...
echo Ovo moze potrajati 1-3 minute...
echo.

pyinstaller --onefile --noconsole --name "UnicateGaming" --distpath "." --workpath "_build" --specpath "_build" launcher.py

if %errorlevel% neq 0 (
    echo.
    echo [GRESKA] Kompajliranje nije uspjelo!
    echo Provjeri da li je Python i PyInstaller instaliran.
    pause
    exit /b 1
)

echo.
echo ============================================
echo    USPJEH! Launcher kompajliran!
echo ============================================
echo.
echo .exe fajl se nalazi ovdje:
echo   %cd%\UnicateGaming.exe
echo.
echo Sada mozes distribuirati UnicateGaming.exe zajedno
echo sa CEF_Client folderom igracima!
echo.
echo Struktura za distribuciju:
echo   UnicateGaming.exe
echo   CEF_Client\          (CEF klijent plugin)
echo   settings.json        (automatski se kreira)
echo.

rem Cleanup
if exist "_build" rmdir /s /q "_build"

pause
