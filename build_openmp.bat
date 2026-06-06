@echo off
title Build for Open.mp - Unicate Gaming
echo ============================================
echo   Compiling fg-ogc.amx for OPEN.MP
echo ============================================
echo.

:: Delete old amx first so we know if recompilation succeeded
if exist "openmp-server\gamemodes\fg-ogc.amx" del "openmp-server\gamemodes\fg-ogc.amx"

:: Compile with OPENMP_BUILD flag defined
:: This replaces SKY plugin natives with open.mp compatible stubs
:: Note: pawncc uses sym=val syntax for defines (NOT -D like C compilers)
::       -i for include path, -o for output, -;+ for semicolon required
pawno\pawncc.exe gamemodes\fg-ogc.pwn OPENMP_BUILD=1 -ipawno\include -oopenmp-server\gamemodes -;+

echo.
if exist "openmp-server\gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\fg-ogc.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo! fg-ogc.amx nije kreiran.
)
echo.
pause
