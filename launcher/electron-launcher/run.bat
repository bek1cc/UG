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
    echo Skini Node.js sa: https://nodejs.org/
    pause
    exit /b 1
)

echo [OK] Node.js pronadjen:
node --version
echo.

:: Set Electron mirror
set ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
set ELECTRON_CUSTOM_DIR={{ version }}

:: Install dependencies if needed
if not exist "node_modules" (
    echo [INFO] Instaliram zavisnosti...
    call npm install
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] npm install nije uspeo! Pokusavam bez mirror-a...
        set ELECTRON_MIRROR=
        rmdir /s /q "node_modules" 2>nul
        call npm install
        if %ERRORLEVEL% NEQ 0 (
            echo [GRESKA] Ne mogu instalirati!
            pause
            exit /b 1
        )
    )
    echo [OK] Zavisnosti instalirane!
    echo.
)

:: Verify Electron binary
if not exist "node_modules\electron\dist\electron.exe" (
    echo [FIX] Electron binary nedostaje, reinstaliram...
    rmdir /s /q "node_modules\electron" 2>nul
    set ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
    call npm install electron --save-dev --force
    if not exist "node_modules\electron\dist\electron.exe" (
        echo [2/2] Pokusavam sa GitHub...
        set ELECTRON_MIRROR=
        rmdir /s /q "node_modules\electron" 2>nul
        call npm install electron --save-dev --force
    )
    if not exist "node_modules\electron\dist\electron.exe" (
        if exist "node_modules\electron" (
            cd node_modules\electron
            node install.js
            cd ..\..
        )
    )
    if not exist "node_modules\electron\dist\electron.exe" (
        echo [GRESKA] Electron binary se ne moze instalirati!
        echo Iskljuci antivirus i pokusaj ponovo.
        pause
        exit /b 1
    )
    echo [OK] Electron instaliran!
    echo.
)

echo [OK] Pokrecem launcher...
echo.

:: Delete old debug log
if exist "launcher_debug.log" del "launcher_debug.log"

:: Run Electron directly from binary (more reliable than npx)
"node_modules\electron\dist\electron.exe" . --no-sandbox 2>&1

echo.
if exist "launcher_debug.log" (
    echo ============================================
    echo DEBUG LOG:
    echo ============================================
    type launcher_debug.log
    echo.
    echo ============================================
)

echo.
echo [INFO] Launcher se zatvorio. Ako se srusio, pogledaj debug log iznad.
pause
