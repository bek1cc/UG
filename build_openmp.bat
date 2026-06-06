@echo off
title Build for Open.mp - Unicate Gaming
echo ============================================
echo   Compiling fg-ogc.amx for OPEN.MP
echo ============================================
echo.

:: Delete old amx first so we know if recompilation succeeded
if exist "gamemodes\fg-ogc.amx" del "gamemodes\fg-ogc.amx"
if exist "openmp-server\gamemodes\fg-ogc.amx" del "openmp-server\gamemodes\fg-ogc.amx"

:: Compile with OPENMP_BUILD flag defined
:: This replaces SKY plugin natives with open.mp compatible stubs
:: Note: pawncc uses sym=val syntax for defines (NOT -D like C compilers)
::       -i for include path, -o for output base name, -;+ for semicolon required
::       -o sets the output FILE PATH (without .amx extension)
pawno\pawncc.exe gamemodes\fg-ogc.pwn OPENMP_BUILD=1 -ipawno\include -oopenmp-server\gamemodes\fg-ogc -;+

echo.
if exist "openmp-server\gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\fg-ogc.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo! fg-ogc.amx nije kreiran.
    echo.
    echo Provjeravam da li je .amx zavrsio u gamemodes\ folderu...
    if exist "gamemodes\fg-ogc.amx" (
        echo [INFO] Pronadjen u gamemodes\fg-ogc.amx - kopiram u openmp-server...
        copy /Y "gamemodes\fg-ogc.amx" "openmp-server\gamemodes\fg-ogc.amx"
        echo [OK] Kopirano u openmp-server\gamemodes\fg-ogc.amx
    )
)
echo.
pause
