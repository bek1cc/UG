@echo off
title Build for Open.mp - Unicate Gaming
echo ============================================
echo   Compiling for OPEN.MP
echo ============================================
echo.

:: ============================================
::  Choose what to build
:: ============================================
echo 1. fg-ogc (full gamemode)
echo 2. test_openmp (minimal test gamemode)
echo.
set /p CHOICE="Izaberi (1 ili 2): "

if "%CHOICE%"=="1" (
    set GAMEMODE=fg-ogc
    set EXTRA_FLAGS=OPENMP_BUILD=1
) else if "%CHOICE%"=="2" (
    set GAMEMODE=test_openmp
    set EXTRA_FLAGS=
) else (
    echo [GRESKA] Nevazeci izbor!
    pause
    exit /b 1
)

echo.
echo Compiling %GAMEMODE%.pwn ...
echo.

:: Delete old amx first
if exist "openmp-server\gamemodes\%GAMEMODE%.amx" del "openmp-server\gamemodes\%GAMEMODE%.amx"
if exist "gamemodes\%GAMEMODE%.amx" del "gamemodes\%GAMEMODE%.amx"

:: Compile
pawno\pawncc.exe gamemodes\%GAMEMODE%.pwn %EXTRA_FLAGS% -ipawno\include -oopenmp-server\gamemodes\%GAMEMODE% -;+

echo.
if exist "openmp-server\gamemodes\%GAMEMODE%.amx" (
    echo [OK] %GAMEMODE%.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\%GAMEMODE%.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo!
    if exist "gamemodes\%GAMEMODE%.amx" (
        echo [INFO] Pronadjen u gamemodes\ - kopiram...
        copy /Y "gamemodes\%GAMEMODE%.amx" "openmp-server\gamemodes\%GAMEMODE%.amx"
    )
)
echo.
pause
