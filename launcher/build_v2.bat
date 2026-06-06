@echo off
title UNICATE GAMING - LAUNCHER Build
color 0B
echo ============================================
echo    UNICATE GAMING - Launcher Compiler
echo ============================================
echo.
echo Kompajlira launcher_v2.py u .exe fajl
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
echo [3/4] Provjeravam fajlove...
if not exist "ug_logo.png" (
    if exist "ug_logo_pro.png" (
        echo Koristim ug_logo_pro.png kao logo.
    ) else if exist "logo.png" (
        echo Koristim logo.png kao logo.
    ) else (
        echo UPOZORENJE: Nema logo fajla!
    )
) else (
    echo Logo pronadjen: ug_logo.png
)

if not exist "ug_icon.ico" (
    echo UPOZORENJE: Nema ug_icon.ico - .exe nece imati ikonicu!
)

echo.
echo [4/4] Kompajliram launcher u .exe...
echo Ovo moze potrajati 1-3 minute...
echo.

pyinstaller --onefile --noconsole ^
    --name "UnicateGaming" ^
    --icon="ug_icon.ico" ^
    --add-data "ug_logo.png;." ^
    --add-data "ug_logo_pro.png;." ^
    --add-data "ug_icon.ico;." ^
    --add-data "ug_logo_glow.png;." ^
    --add-data "bg_gta.png;." ^
    --distpath "." ^
    --workpath "_build" ^
    --specpath "_build" ^
    --clean launcher_v2.py

if %errorlevel% neq 0 (
    echo.
    echo [GRESKA] Kompajliranje nije uspjelo!
    pause
    exit /b 1
)

echo.
echo ============================================
echo    USPJEH! Launcher kompajliran!
echo ============================================
echo.
echo .exe fajl: %cd%\UnicateGaming.exe
echo.
echo Za distribuciju igracima trebas samo:
echo   UnicateGaming.exe (sve je u jednom fajlu)
echo.

if exist "_build" rmdir /s /q "_build"
pause
