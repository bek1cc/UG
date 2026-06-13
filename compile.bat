@echo off
title UG - Compile + Start (SA-MP 0.3.DL)
echo ============================================
echo   UNICATE GAMING - Compile + Start Server
echo   SA-MP 0.3.DL Edition
echo ============================================
echo.

REM Kill any running server
taskkill /f /im samp-server.exe >nul 2>nul
timeout /t 1 /nobreak >nul

REM === DELETE OLD .amx ===
del /f /q "gamemodes\fg-ogc.amx" 2>nul
del /f /q "fg-ogc.amx" 2>nul

REM === COMPILE (using pawno compiler) ===
echo Compiling fg-ogc.pwn for SA-MP 0.3.DL...
echo.
pawno\pawncc.exe gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -ipawno\include -;+

echo.

REM === FIND AND MOVE .amx ===
if exist "gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx compiled successfully!
) else if exist "fg-ogc.amx" (
    move /Y "fg-ogc.amx" "gamemodes\fg-ogc.amx" >nul
    echo [OK] fg-ogc.amx moved to gamemodes\
) else (
    echo [ERROR] Compilation failed! Check errors above.
    pause
    exit /b 1
)

REM === VERIFY libmysql.dll ===
if not exist "libmysql.dll" (
    echo [WARNING] libmysql.dll not found in server root!
    echo           MySQL plugin will NOT load without it.
    echo           Download from: https://github.com/peters1v/SA-MP-mysql-plugin/raw/master/libmysql.dll
    echo           Place libmysql.dll in the same folder as samp-server.exe
    echo.
)

REM === ENSURE RUNTIME DIRS EXIST ===
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Streljane Logovi Inventory PhonePins) do (
    if not exist "scriptfiles\%%d" mkdir "scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles dirs ready

REM === COPY MAP DATA FILES ===
if exist "scriptfiles\SAmin.hmap" echo [OK] SAmin.hmap found
if exist "scriptfiles\SAfull.hmap" echo [OK] SAfull.hmap found
echo [OK] Map data check done

REM === VERIFY ===
echo.
if not exist "gamemodes\fg-ogc.amx" (
    echo [ERROR] fg-ogc.amx not found!
    pause
    exit /b 1
)

echo ============================================
echo   Ready! Starting SA-MP 0.3.DL server...
echo ============================================
echo.
echo   Port: 7777
echo   Custom models: models/ folder
echo   Plugins: crashdetect, streamer, sscanf, iTD, MapAndreas, mysql
echo.
echo   IMPORTANT: Make sure libmysql.dll is in this folder!
echo.
pause
samp-server.exe
