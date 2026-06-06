@echo off
title Unicate Gaming - Upload na Server
color 0B

echo ============================================
echo    UNICATE GAMING - UPLOAD NA SERVER
echo ============================================
echo.
echo Ova skripta uploada kompajlirane fajlove
echo na tvoj server (135.125.156.197)
echo.

:: Provjeri da li ima WinSCP ili koristi scp
:: Ovdje koristimo pscp (PuTTY SCP) ako imas instaliran
:: Ako nemas, koristi rucno FTP/FileZilla

echo Uploadam gamemode...
pscp -r openmp-server\gamemodes\fg-ogc.amx root@135.125.156.197:/path/to/server/gamemodes/

echo Uploadam filterscripts...
pscp -r openmp-server\filterscripts\*.amx root@135.125.156.197:/path/to/server/filterscripts/

echo Uploadam scriptfiles ako ima promjena...
echo (Ovo radi samo ako imas pscp instaliran)
echo.

echo Ako nemas pscp, koristi FileZilla ili WinSCP:
echo   Host: 135.125.156.197
echo   User: root
echo   Password: (tvoj password)
echo   Port: 22
echo.
echo Uploadaj SADRZAJ openmp-server foldera na server!
echo.
pause
