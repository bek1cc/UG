@echo off
title Build for Open.mp - Unicate Gaming
echo ============================================
echo   Compiling for OPEN.MP
echo ============================================
echo.

:: Delete old amx first
if exist "openmp-server\gamemodes\fg-ogc.amx" del "openmp-server\gamemodes\fg-ogc.amx"
if exist "gamemodes\fg-ogc.amx" del "gamemodes\fg-ogc.amx"

:: Compile
echo Compiling fg-ogc.pwn ...
pawno\pawncc.exe gamemodes\fg-ogc.pwn OPENMP_BUILD=1 -igamemodes -igamemodes\maps -igamemodes\systems -ipawno\include -oopenmp-server\gamemodes\fg-ogc -;+

echo.
if exist "openmp-server\gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\fg-ogc.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo!
    if exist "gamemodes\fg-ogc.amx" (
        echo [INFO] Pronadjen u gamemodes\ - kopiram...
        copy /Y "gamemodes\fg-ogc.amx" "openmp-server\gamemodes\fg-ogc.amx"
    )
)
echo.
pause
