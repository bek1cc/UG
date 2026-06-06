@echo off
title Unicate Gaming Launcher - Setup
echo ========================================
echo   UNICATE GAMING LAUNCHER v3.0
echo ========================================
echo.

:: Check if Node.js is installed
where node >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [GRESKA] Node.js nije instaliran!
    echo.
    echo Skini Node.js sa: https://nodejs.org/
    echo Skini LTS verziju i instaliraj je.
    echo Posle instalacije restartuj racunar i pokreni ovu skriptu ponovo.
    echo.
    pause
    exit /b 1
)

echo [OK] Node.js pronadjen:
node --version
echo.

:: Check if node_modules exists
if not exist "node_modules" (
    echo [INFO] Instaliram zavisnosti... (prvi put moze potrajati)
    call npm install
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] npm install nije uspeo!
        echo Pokusaj rucno: npm install
        pause
        exit /b 1
    )
    echo [OK] Zavisnosti instalirane!
    echo.
) else (
    echo [OK] Zavisnosti vec instalirane.
    echo.
)

echo [INFO] Pokrecem launcher...
call npx electron .
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [GRESKA] Launcher nije mogao da se pokrene!
    echo Pokusaj rucno: npx electron .
    pause
    exit /b 1
)
