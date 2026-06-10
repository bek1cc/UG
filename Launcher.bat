@echo off
title Unicate Gaming Launcher
color 0B

:: Idi u launcher folder
cd /d "%~dp0launcher\electron-launcher"

:: Provjeri Node.js
where node >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ============================================
    echo    NODE.JS NIJE INSTALIRAN!
    echo ============================================
    echo.
    echo Skini Node.js sa: https://nodejs.org/
    echo (Skini LTS verziju, instaliraj i pokreni ponovo)
    echo.
    start https://nodejs.org/
    pause
    exit /b 1
)

:: Prvi put - instaliraj npm pakete
if not exist "node_modules" (
    echo [1/2] Instaliram pakete (samo prvi put)...
    call npm install --ignore-scripts
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] npm install failed!
        pause
        exit /b 1
    )
    echo [OK] Paketi instalirani!
    echo.
)

:: Provjeri da li electron binary postoji
if not exist "node_modules\electron\dist\electron.exe" (
    echo [2/2] Skidam Electron binary (~80MB, samo prvi put)...
    call node node_modules\electron\install.js
    if not exist "node_modules\electron\dist\electron.exe" (
        echo [GRESKA] Electron se nije skinuo!
        echo Pokreni: cd launcher\electron-launcher && npm install
        pause
        exit /b 1
    )
)

:: Pokreni launcher
echo.
echo Pokrecem launcher...
echo.
start "" "node_modules\electron\dist\electron.exe" . --no-sandbox
