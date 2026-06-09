@echo off
title Unicate Gaming - Prvi Setup
color 0B

echo ============================================
echo    UNICATE GAMING - LOKALNI SETUP
echo ============================================
echo.

:: 1. Provjeri da li je open.mp vec tu
if exist "open.mp\omp-server.exe" (
    echo [OK] open.mp server vec postoji!
    goto :compile
)

:: 2. Provjeri da li je zip tu
if exist "omp-windows.zip" (
    echo [1/3] Extractam open.mp iz zip-a...
    powershell -Command "Expand-Archive -Path 'omp-windows.zip' -DestinationPath 'omp-temp' -Force"
    if exist "omp-temp\Server\omp-server.exe" (
        xcopy /E /I /Y "omp-temp\Server" "open.mp"
        rmdir /S /Q "omp-temp"
        echo [OK] open.mp server extractan!
    ) else (
        echo [GRESKA] Ne mogu naci omp-server.exe u zip-u!
        echo Skini open.mp rucno sa https://open.mp/downloads
        echo i extractaj u UG\open.mp\ folder
        pause
        exit /b 1
    )
    goto :compile
)

:: 3. Skini open.mp
echo [1/3] Skidam open.mp server sa interneta...
echo (Ovo moze potrajati...)
powershell -Command "& { Invoke-WebRequest -Uri 'https://github.com/openmultiplayer/open.mp/releases/latest/download/omp-windows.zip' -OutFile 'omp-windows.zip' }"
if exist "omp-windows.zip" (
    echo [OK] Skinuto! Extractam...
    powershell -Command "Expand-Archive -Path 'omp-windows.zip' -DestinationPath 'omp-temp' -Force"
    if exist "omp-temp\Server\omp-server.exe" (
        xcopy /E /I /Y "omp-temp\Server" "open.mp"
        rmdir /S /Q "omp-temp"
        echo [OK] open.mp server spreman!
    ) else (
        xcopy /E /I /Y "omp-temp\omp-server.exe" "open.mp" 2>nul
        rmdir /S /Q "omp-temp" 2>nul
    )
) else (
    echo [GRESKA] Ne mogu skinuti open.mp!
    echo Skini rucno sa https://open.mp/downloads
    echo i extractaj u UG\open.mp\ folder
    pause
    exit /b 1
)

:compile
:: 4. Kopiraj potrebne fajlove u open.mp
echo.
echo [2/3] Kopiram scriptfiles i plugini u open.mp...

if not exist "open.mp\scriptfiles" xcopy /E /I /Y "scriptfiles" "open.mp\scriptfiles"

:: FIX: Ensure Korisnici directory exists for account file saving
if not exist "open.mp\scriptfiles\Korisnici" mkdir "open.mp\scriptfiles\Korisnici"
if not exist "scriptfiles\Korisnici" mkdir "scriptfiles\Korisnici"
if not exist "open.mp\plugins" xcopy /E /I /Y "plugins" "open.mp\plugins" 2>nul
if not exist "open.mp\npcmodes" xcopy /E /I /Y "npcmodes" "open.mp\npcmodes"

:: 5. Kopiraj config
if exist "config.json" copy /Y "config.json" "open.mp\config.json"

:: 6. Kompajlaj
echo.
echo [3/3] Kompajlam gamemode...
call compile.bat

echo.
echo ============================================
echo    GOTOVO! Server je spreman.
echo    Pokreni: open.mp\omp-server.exe
echo    Igra:    127.0.0.1:7777
echo ============================================
echo.
pause
