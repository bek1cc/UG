@echo off
title UG - Compile + Start
echo ============================================
echo   UNICATE GAMING - Compile + Start Server
echo ============================================
echo.

REM Kill any running server
taskkill /f /im omp-server.exe >nul 2>nul
timeout /t 1 /nobreak >nul

REM OpenMP folder name (with space and hash)
set OMP=# open.mp

REM === YSI includes are now pre-patched in repo ===
REM No runtime patching needed - all #error Did you use lines are already commented out
REM Gamemode uses new YSI paths: YSI_Storage\y_ini, YSI_Data\y_iterate, YSI_Coding\y_timers

REM === DELETE OLD .amx ===
del /f /q "%OMP%\gamemodes\fg-ogc.amx" 2>nul
del /f /q "gamemodes\fg-ogc.amx" 2>nul
del /f /q "fg-ogc.amx" 2>nul

REM === DELETE OLD CONFLICTING INCLUDES ===
REM Old foreach.inc conflicts with YSI y_iterate/y_foreach
del /f /q "gamemodes\foreach.inc" 2>nul
del /f /q "gamemodes\maps\foreach.inc" 2>nul
del /f /q "gamemodes\systems\foreach.inc" 2>nul
del /f /q "%OMP%\qawno\include\foreach.inc" 2>nul
del /f /q "%OMP%\qawno\upgrader\foreach.inc" 2>nul

REM === COMPILE (using qawno compiler from open.mp) ===
echo Compiling fg-ogc.pwn with qawno...
echo.
"%OMP%\qawno\pawncc.exe" gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -i"%OMP%\qawno\include" -i"%OMP%\qawno\upgrader" -;+

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

REM === COPY TO OPEN.MP ===
if not exist "%OMP%\gamemodes" mkdir "%OMP%\gamemodes"
copy /Y "gamemodes\fg-ogc.amx" "%OMP%\gamemodes\fg-ogc.amx" >nul
echo [OK] fg-ogc.amx deployed to %OMP%\gamemodes\

REM === COPY PLUGINS (including mysql and SKY) ===
if not exist "%OMP%\plugins" mkdir "%OMP%\plugins"
for %%p in (crashdetect streamer sscanf iTD MapAndreas mysql SKY) do (
    if exist "plugins\%%p.dll" copy /Y "plugins\%%p.dll" "%OMP%\plugins\" >nul 2>nul
)
echo [OK] Plugins deployed

REM === COPY CEF SCRIPTFILES ===
if exist "scriptfiles\cef" (
    if not exist "%OMP%\scriptfiles\cef" mkdir "%OMP%\scriptfiles\cef"
    xcopy /E /I /Y /Q "scriptfiles\cef" "%OMP%\scriptfiles\cef" >nul 2>nul
)
echo [OK] CEF resources deployed

REM === COPY STATIC SCRIPTFILES (only if open.mp dirs don't exist) ===
for %%d in (Organizacije Dileri Firme AutoSaloni Garages Kontejneri Kapije Parkinzi Pumpe Bankomati Events GPS Granice Furniture Aktori Poslovi Radars Ulice Zones) do (
    if not exist "%OMP%\scriptfiles\%%d" (
        if exist "scriptfiles\%%d" (
            xcopy /E /I /Y /Q "scriptfiles\%%d" "%OMP%\scriptfiles\%%d" >nul 2>nul
        )
    )
)

REM === ENSURE RUNTIME DIRS EXIST ===
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Streljane Logovi Inventory PhonePins) do (
    if not exist "%OMP%\scriptfiles\%%d" mkdir "%OMP%\scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles synced

REM === COPY MAP DATA FILES ===
if exist "scriptfiles\SAmin.hmap" copy /Y "scriptfiles\SAmin.hmap" "%OMP%\scriptfiles\" >nul 2>nul
if exist "scriptfiles\SAfull.hmap" copy /Y "scriptfiles\SAfull.hmap" "%OMP%\scriptfiles\" >nul 2>nul
if exist "scriptfiles\skins.txt" copy /Y "scriptfiles\skins.txt" "%OMP%\scriptfiles\" >nul 2>nul
echo [OK] Map data synced

REM === VERIFY ===
echo.
if not exist "%OMP%\gamemodes\fg-ogc.amx" (
    echo [ERROR] fg-ogc.amx not found in %OMP%\gamemodes\!
    pause
    exit /b 1
)
echo ============================================
echo   Ready! Starting open.mp server...
echo ============================================
echo.
cd /d "%~dp0%OMP%"
omp-server.exe
