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
:: Note: -I (capital I) for include path, -D for define
pawno\pawncc.exe gamemodes\fg-ogc.pwn -DOPENMP_BUILD -Ipawno\include -oopenmp-server\gamemodes -;+

echo.
if exist "openmp-server\gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\fg-ogc.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo! fg-ogc.amx nije kreiran.
)
echo.
pause
