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

:: Set Electron mirror for faster/more reliable download
set ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
set ELECTRON_CUSTOM_DIR={{ version }}

:: Step 1: If node_modules doesn't exist, do fresh install
if not exist "node_modules" (
    echo [INFO] Instaliram zavisnosti (prvi put)...
    call npm install
    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo [GRESKA] npm install nije uspeo!
        echo Pokusavam sa default mirror-om...
        set ELECTRON_MIRROR=
        rmdir /s /q "node_modules" 2>nul
        call npm install
        if %ERRORLEVEL% NEQ 0 (
            echo [GRESKA] Ne mogu instalirati zavisnosti!
            echo Pokusaj rucno u CMD:
            echo   cd /d "%~dp0"
            echo   npm install
            echo.
            pause
            exit /b 1
        )
    )
    echo [OK] Zavisnosti instalirane!
    echo.
)

:: Step 2: Verify Electron binary exists
if exist "node_modules\electron\dist\electron.exe" (
    echo [OK] Electron binary pronadjen!
    echo.
    goto :launch
)

:: Step 3: Electron binary missing - try to fix
echo [FIX] Electron binary nedostaje, pokusavam popravku...
echo.

:: Method 1: Delete electron folder and reinstall with mirror
echo [1/3] Pokusavam sa npmmirror...
rmdir /s /q "node_modules\electron" 2>nul
set ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
set ELECTRON_CUSTOM_DIR={{ version }}
call npm install electron --save-dev --force
if exist "node_modules\electron\dist\electron.exe" (
    echo [OK] Electron instaliran sa npmmirror!
    echo.
    goto :launch
)

:: Method 2: Try GitHub releases directly
echo [2/3] Pokusavam sa GitHub releases...
rmdir /s /q "node_modules\electron" 2>nul
set ELECTRON_MIRROR=
set ELECTRON_CUSTOM_DIR=
call npm install electron --save-dev --force
if exist "node_modules\electron\dist\electron.exe" (
    echo [OK] Electron instaliran sa GitHub!
    echo.
    goto :launch
)

:: Method 3: Run electron's install script manually
echo [3/3] Pokusavam rucnu instalaciju Electron binary-ja...
if exist "node_modules\electron" (
    cd node_modules\electron
    node install.js
    cd ..\..
    if exist "node_modules\electron\dist\electron.exe" (
        echo [OK] Electron binary rucno instaliran!
        echo.
        goto :launch
    )
)

:: All methods failed
echo.
echo ============================================
echo [GRESKA] Electron se ne moze instalirati!
echo ============================================
echo.
echo Moguci razlozi:
echo   1. Antivirus blokira download - iskljuci ga privremeno
echo   2. Firewall blokira pristup - dozvoli node.exe u firewall-u
echo   3. Nema internet konekcije
echo.
echo Rucno resenje - pokreni u CMD kao Administrator:
echo   cd /d "%~dp0"
echo   rmdir /s /q node_modules
echo   set ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
echo   npm install
echo.
pause
exit /b 1

:launch
echo [OK] Pokrecem launcher...
echo.
echo [INFO] Ako se launcher odmah ugasi, vidjeces gresku ispod.
echo.

:: Run Electron with --no-sandbox for Windows compatibility
call npx electron . --no-sandbox 2>&1
set EXIT_CODE=%ERRORLEVEL%

echo.
if %EXIT_CODE% NEQ 0 (
    echo ============================================
    echo [GRESKA] Launcher se srusio! (Exit code: %EXIT_CODE%)
    echo ============================================
    echo.
    echo Pokusaj:
    echo   1. Obrisi node_modules folder i pokreni run.bat ponovo
    echo   2. Iskljuci antivirus i pokusaj ponovo
    echo   3. Pokreni rucno u CMD:
    echo      cd /d "%~dp0"
    echo      npx electron . --no-sandbox
    echo.
)

:: Keep window open so user can see errors
pause
