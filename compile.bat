@echo off
title UG - Compile + Start (Open.mp + CEF)
echo ============================================
echo   UNICATE GAMING - Compile + Start Server
echo   Open.mp Edition sa CEF pluginom
echo ============================================
echo.

REM Kill any running server
taskkill /f /im omp-server.exe >nul 2>nul
timeout /t 1 /nobreak >nul

REM === DELETE OLD .amx ===
del /f /q "open.mp\gamemodes\fg-ogc.amx" 2>nul
del /f /q "gamemodes\fg-ogc.amx" 2>nul

REM === COMPILE (using open.mp qawno compiler) ===
echo Compiling fg-ogc.pwn za Open.mp sa CEF...
echo.
open.mp\qawno\pawncc.exe gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -ipawno\include -iopen.mp\qawno\include -;+

echo.

REM === FIND AND MOVE .amx ===
if exist "gamemodes\fg-ogc.amx" (
    move /Y "gamemodes\fg-ogc.amx" "open.mp\gamemodes\fg-ogc.amx" >nul
    echo [OK] fg-ogc.amx compiled and moved to open.mp\gamemodes\
) else (
    echo [ERROR] Compilation failed! Check errors above.
    pause
    exit /b 1
)

REM === VERIFY MySQL ===
if not exist "open.mp\libmariadb.dll" (
    echo [WARNING] libmariadb.dll not found in open.mp\ folder!
    echo           MySQL R41 plugin needs libmariadb.dll (not libmysql.dll!)
    echo           Download from: https://github.com/pBlueG/SA-MP-MySQL/releases/download/R41-4/mysql-R41-4-win32.zip
    echo.
)

REM === VERIFY CEF PLUGIN ===
if not exist "open.mp\plugins\cef.dll" (
    echo [WARNING] cef.dll not found in open.mp\plugins\!
    echo           CEF plugin will NOT load without it.
    echo.
)

REM === COPY PLUGINS TO open.mp IF NEEDED ===
if not exist "open.mp\plugins" mkdir "open.mp\plugins" 2>nul
for %%p in (cef.dll crashdetect.dll streamer.dll sscanf.dll iTD.dll MapAndreas.dll mysql.dll) do (
    if exist "plugins\%%p" (
        copy /Y "plugins\%%p" "open.mp\plugins\%%p" >nul 2>nul
    )
)
echo [OK] Plugins synced to open.mp\plugins\

REM === ENSURE RUNTIME DIRS EXIST ===
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Streljane Logovi Inventory PhonePins) do (
    if not exist "open.mp\scriptfiles\%%d" mkdir "open.mp\scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles dirs ready

REM === COPY CEF RESOURCES IF NEEDED ===
if not exist "open.mp\scriptfiles\cef" (
    if exist "scriptfiles\cef" (
        xcopy /E /I /Y "scriptfiles\cef" "open.mp\scriptfiles\cef" >nul
        echo [OK] CEF resources copied to open.mp\scriptfiles\cef\
    )
)

REM === COPY MAP DATA FILES ===
if not exist "open.mp\scriptfiles\SAmin.hmap" (
    if exist "scriptfiles\SAmin.hmap" copy /Y "scriptfiles\SAmin.hmap" "open.mp\scriptfiles\" >nul
)
if not exist "open.mp\scriptfiles\SAfull.hmap" (
    if exist "scriptfiles\SAfull.hmap" copy /Y "scriptfiles\SAfull.hmap" "open.mp\scriptfiles\" >nul
)
echo [OK] Map data check done

REM === COPY ARTWORK MODELS ===
if not exist "open.mp\artwork" mkdir "open.mp\artwork" 2>nul
if exist "artwork" (
    xcopy /Y "artwork\*.dff" "open.mp\artwork\" >nul 2>nul
    xcopy /Y "artwork\*.txd" "open.mp\artwork\" >nul 2>nul
    echo [OK] Artwork models synced
)

REM === VERIFY ===
echo.
if not exist "open.mp\gamemodes\fg-ogc.amx" (
    echo [ERROR] fg-ogc.amx not found in open.mp\gamemodes\!
    pause
    exit /b 1
)

echo ============================================
echo   Ready! Starting Open.mp server sa CEF...
echo ============================================
echo.
echo   Port: 7777
echo   Plugins: crashdetect, cef, streamer, sscanf, iTD, MapAndreas, mysql
echo   CEF: ENABLED (phone, tablet, inventory, laptop, portal, amenu, case, cardcase, dog)
echo   Artwork: ENABLED (custom models via AddSimpleModel - 0.3.DL clients)
echo.
echo   NOTE: Players use SA-MP 0.3.DL client (auto-downloaded by launcher)
echo   NOTE: libmariadb.dll is needed in open.mp\ for MySQL R41
echo.
pause
cd open.mp
omp-server.exe
