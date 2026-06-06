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

:: ============================================================
::  STEP 1: Install npm dependencies
:: ============================================================
if not exist "node_modules" (
    echo [1/3] Instaliram npm pakete...
    call npm install --ignore-scripts
    if %ERRORLEVEL% NEQ 0 (
        echo [GRESKA] npm install nije uspeo!
        pause
        exit /b 1
    )
    echo [OK] npm paketi instalirani!
    echo.
)

:: ============================================================
::  STEP 2: Check if Electron binary exists
:: ============================================================
if exist "node_modules\electron\dist\electron.exe" (
    echo [OK] Electron binary pronadjen!
    echo.
    goto :launch
)

:: ============================================================
::  STEP 3: Download Electron binary manually with PowerShell
:: ============================================================
echo.
echo [FIX] Electron binary nedostaje - skidam rucno...
echo.

:: Read Electron version from node_modules/electron/package.json using Node.js
set ELECTRON_VER=
for /f "delims=" %%v in ('node -e "try{console.log(require('./node_modules/electron/package.json').version)}catch(e){console.log('')}" 2^>nul') do set ELECTRON_VER=%%v

echo [INFO] Electron verzija: %ELECTRON_VER%

if "%ELECTRON_VER%"=="" (
    echo [GRESKA] Ne mogu pronaci Electron verziju!
    echo Pokusavam fiksnu verziju 28.3.3...
    set ELECTRON_VER=28.3.3
)

:: Download URLs
set ZIP_NAME=electron-v%ELECTRON_VER%-win32-x64.zip
set URL1=https://github.com/electron/electron/releases/download/v%ELECTRON_VER%/%ZIP_NAME%
set URL2=https://npmmirror.com/mirrors/electron/v%ELECTRON_VER%/%ZIP_NAME%

echo.
echo [2/3] Skidam Electron v%ELECTRON_VER% (~80MB)...
echo       URL: %URL1%
echo.

:: Download with PowerShell - show progress
powershell -Command "& { [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $url = '%URL1%'; $file = 'electron_tmp.zip'; try { Write-Host '[PS] Skidam sa GitHub...'; $wc = New-Object System.Net.WebClient; $wc.DownloadFile($url, $file); Write-Host '[PS] Gotovo!'; } catch { Write-Host '[PS] GitHub fail, pokusavam npmmirror...'; $url2 = '%URL2%'; try { $wc.DownloadFile($url2, $file); Write-Host '[PS] Gotovo sa npmmirror!'; } catch { Write-Host '[PS] GRESKA:' $_.Exception.Message; exit 1; } } }"

if not exist "electron_tmp.zip" (
    echo.
    echo [GRESKA] Ne mogu skinuti Electron binary!
    echo.
    echo Pokusaj rucno:
    echo   1. Skini: %URL1%
    echo   2. Raspakuj u: node_modules\electron\dist\
    echo   3. Pokreni run.bat ponovo
    echo.
    pause
    exit /b 1
)

:: Check file size
for %%A in ("electron_tmp.zip") do set ZIP_SIZE=%%~zA
echo [INFO] Skinuti fajl: %ZIP_SIZE% bajtova

if %ZIP_SIZE% LSS 1000000 (
    echo [GRESKA] Fajl je premali - verovatno nije skinut ispravno!
    del "electron_tmp.zip" 2>nul
    pause
    exit /b 1
)

echo.
echo [3/3] Raspakujem Electron binary...

:: Create dist directory
if not exist "node_modules\electron\dist" mkdir "node_modules\electron\dist"

:: Extract with PowerShell
powershell -Command "& { try { Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::ExtractToDirectory((Resolve-Path 'electron_tmp.zip').Path, (Resolve-Path 'node_modules\electron\dist').Path); Write-Host '[PS] Raspakivanje zavrseno!'; } catch { Write-Host '[PS] GRESKA:' $_.Exception.Message; exit 1; } }"

:: Clean up
del "electron_tmp.zip" 2>nul

:: Verify
if exist "node_modules\electron\dist\electron.exe" (
    echo.
    echo [OK] Electron binary uspjesno instaliran!
    echo.
) else (
    echo.
    echo [GRESKA] electron.exe nije pronadjen nakon raspakivanja!
    echo Proveri node_modules\electron\dist\ folder
    echo.
    pause
    exit /b 1
)

:: ============================================================
::  LAUNCH
:: ============================================================
:launch
echo ========================================
echo   Pokrecem launcher...
echo ========================================
echo.

:: Delete old debug log
if exist "launcher_debug.log" del "launcher_debug.log"

:: Run Electron
"node_modules\electron\dist\electron.exe" . --no-sandbox 2>&1

echo.
if exist "launcher_debug.log" (
    echo ============================================
    echo   DEBUG LOG:
    echo ============================================
    type launcher_debug.log
    echo.
)

echo.
pause
