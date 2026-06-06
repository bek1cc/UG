@echo off
title Unicate Gaming Launcher - Build
echo ============================================
echo   Unicate Gaming Launcher - Build Script
echo ============================================
echo.

echo [1/3] Provjeravam Python...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Python nije instaliran! Preuzmi sa https://python.org
    pause
    exit /b 1
)
echo Python pronadjen!

echo.
echo [2/3] Instaliram zavisnosti...
pip install pyinstaller pillow requests --quiet

echo.
echo [3/3] Kompajliram launcher...
pyinstaller --onefile --windowed --name="Unicate Gaming" --clean --noconfirm launcher.py

echo.
echo ============================================
echo   Build zavrsen!
echo   EXE fajl: dist\Unicate Gaming.exe
echo ============================================
echo.
echo Kopiraj "Unicate Gaming.exe" i daj ga svim igracima.
pause
