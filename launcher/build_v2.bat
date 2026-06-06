@echo off
title Unicate Gaming Launcher V2 - Build EXE
color 0A
echo ============================================
echo    UNICATE GAMING - Launcher V2 Compiler
echo ============================================
echo.
echo Ova skripta ce kompajlirati launcher_v2.py u .exe fajl
echo tako da igraci ne trebaju imati Python instaliran.
echo.

echo [1/4] Provjeravam Python...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Python nije instaliran! Preuzmi sa https://python.org
    pause
    exit /b 1
)
echo Python pronadjen!

echo.
echo [2/4] Instaliram zavisnosti...
pip install pyinstaller customtkinter pillow requests --quiet

echo.
echo [3/4] Kopiram logo...
if not exist "logo.png" (
    if exist "ug_logo.png" (
        echo Koristim ug_logo.png kao logo.
    ) else (
        echo UPOZORENJE: Nema logo fajla! Launcher ce raditi bez slike.
    )
)

echo.
echo [4/4] Kompajliram launcher u .exe...
echo Ovo moze potrajati 1-3 minute...
echo.

pyinstaller --onefile --noconsole --name "UnicateGaming" --add-data "logo.png;." --add-data "ug_logo.png;." --distpath "." --workpath "_build" --specpath "_build" --clean launcher_v2.py

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
echo Za distribuciju igracima trebas:
echo   1. UnicateGaming.exe
echo   2. logo.png (u istom folderu kao .exe)
echo.
echo Ili mozes kompajlirati sa --onefile da sve bude u jednom .exe
echo.

rem Cleanup
if exist "_build" rmdir /s /q "_build"

pause
