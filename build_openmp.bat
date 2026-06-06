@echo off
title Build for Open.mp - Unicate Gaming
echo ============================================
echo   Compiling fg-ogc.amx for OPEN.MP
echo ============================================
echo.

:: Compile with OPENMP_BUILD flag defined
:: This replaces SKY plugin natives with open.mp compatible stubs
pawno\pawncc.exe gamemodes\fg-ogc.pwn -DOPENMP_BUILD -i pawno\include -o openmp-server\gamemodes -;+

echo.
if exist "openmp-server\gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled for open.mp!
    echo [OK] Saved to: openmp-server\gamemodes\fg-ogc.amx
) else (
    echo [GRESKA] Kompajliranje nije uspjelo!
)
echo.
pause
