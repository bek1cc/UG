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
    echo.
    pause
    exit /b 1
)

echo [OK] Node.js pronadjen:
node --version
echo.

:: Fix Electron if it failed to install
if exist "node_modules\electron" (
    if not exist "node_modules\electron\dist\electron.exe" (
        echo [FIX] Electron binary nedostaje, reinstaliram...
        rmdir /s /q "node_modules\electron" 2>nul
        call npm install electron --save-dev
        if %ERRORLEVEL% NEQ 0 (
            echo [GRESKA] Ne mogu instalirati Electron!
            echo Pokusaj rucno u CMD:
            echo   cd /d "%~dp0"
            echo   rmdir /s /q node_modules\electron
            echo   npm install electron --save-dev
            echo.
            pause
            exit /b 1
        )
        echo [OK] Electron reinstaliran!
        echo.
    )
)

:: Install all dependencies if needed
if not exist "node_modules" (
    echo [INFO] Instaliram zavisnosti...
    call npm install
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] npm install nije uspeo!
        pause
        exit /b 1
    )
    echo [OK] Zavisnosti instalirane!
    echo.
)

:: Verify Electron binary exists before launching
if not exist "node_modules\electron\dist\electron.exe" (
    echo [FIX] Electron binary i dalje nedostaje, pokusavam ponovo...
    rmdir /s /q "node_modules\electron" 2>nul
    call npm install electron --save-dev
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] Electron se ne moze instalirati!
        echo Mozda antivirus blokira download. Iskljuci antivirus i pokusaj ponovo.
        echo.
        pause
        exit /b 1
    )
)

echo [OK] Pokrecem launcher...
echo.
call npx electron .
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [GRESKA] Launcher nije mogao da se pokrene!
    echo Pokusaj rucno:
    echo   cd /d "%~dp0"
    echo   npx electron .
    echo.
    pause
    exit /b 1
)
