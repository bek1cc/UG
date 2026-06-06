@echo off
title Unicate Gaming - Build Script
color 0A

echo ============================================
echo    UNICATE GAMING - BUILD SCRIPT
echo ============================================
echo.

:: 1. Kompajliraj glavni gamemode sa pawno
echo [1/3] Kompajliram fg-ogc.pwn...
pawno\pawncc.exe gamemodes\fg-ogc.pwn -i pawno\include -;+ -(+ +2
echo.

:: 2. Kopiraj .amx u open.mp server folder
echo [2/3] Kopiram fg-ogc.amx u open.mp server folder...
copy /Y gamemodes\fg-ogc.amx openmp-server\gamemodes\fg-ogc.amx
echo.

:: 3. Kompajliraj filterscripts
echo [3/3] Kompajliram filterscripts...
if exist filterscripts\anims.pwn (
    pawno\pawncc.exe filterscripts\anims.pwn -i pawno\include -;+ -(+ +2
    copy /Y filterscripts\anims.amx openmp-server\filterscripts\anims.amx
)
echo.

echo ============================================
echo    BUILD GOTOV!
echo    .amx fajlovi su u openmp-server folderu
echo    Sada uploadaj openmp-server na server
echo ============================================
echo.
pause
