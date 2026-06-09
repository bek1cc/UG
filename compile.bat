@echo off
title UG - Compile + Start
echo ============================================
echo   UNICATE GAMING - Compile + Start Server
echo ============================================
echo.

REM Kill any running server
taskkill /f /im omp-server.exe >nul 2>nul
timeout /t 1 /nobreak >nul

REM OpenMP folder name
set OMP=# open.mp

REM === DELETE OLD .amx ===
del /f /q "%OMP%\gamemodes\fg-ogc.amx" 2>nul
del /f /q "gamemodes\fg-ogc.amx" 2>nul
del /f /q "fg-ogc.amx" 2>nul

REM === COMPILE (using pawno compiler) ===
echo Compiling fg-ogc.pwn...
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

REM === COPY TO OPEN.MP ===
if not exist "%OMP%\gamemodes" mkdir "%OMP%\gamemodes"
copy /Y "gamemodes\fg-ogc.amx" "%OMP%\gamemodes\fg-ogc.amx" >nul
echo [OK] fg-ogc.amx deployed to %OMP%\gamemodes\

REM === COPY PLUGINS ===
if not exist "%OMP%\plugins" mkdir "%OMP%\plugins"
for %%p in (crashdetect streamer sscanf iTD MapAndreas) do (
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
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Streljane Logovi Inventory) do (
    if not exist "%OMP%\scriptfiles\%%d" mkdir "%OMP%\scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles synced

REM === VERIFY ===
echo.
echo ============================================
echo   Ready! Starting open.mp server...
echo ============================================
echo.
cd /d "%~dp0%OMP%"
omp-server.exe
