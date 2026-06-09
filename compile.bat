@echo off
title UG - Compile + Start
echo ============================================
echo   UNICATE GAMING - Compile + Start Server
echo ============================================
echo.

REM Kill any running server
taskkill /f /im omp-server.exe >nul 2>nul
timeout /t 1 /nobreak >nul

REM === DELETE OLD .amx ===
del /f /q "open.mp\gamemodes\fg-ogc.amx" 2>nul
del /f /q "gamemodes\fg-ogc.amx" 2>nul
del /f /q "fg-ogc.amx" 2>nul

REM === COMPILE (using open.mp qawno compiler) ===
echo Compiling fg-ogc.pwn with qawno...
echo.
"open.mp\qawno\pawncc.exe" gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -i"open.mp\qawno\include" -;+

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
if not exist "open.mp\gamemodes" mkdir "open.mp\gamemodes"
copy /Y "gamemodes\fg-ogc.amx" "open.mp\gamemodes\fg-ogc.amx" >nul
echo [OK] fg-ogc.amx deployed to open.mp\gamemodes\

REM === COPY PLUGINS ===
if not exist "open.mp\plugins" mkdir "open.mp\plugins"
for %%p in (crashdetect streamer sscanf iTD MapAndreas) do (
    if exist "plugins\%%p.dll" copy /Y "plugins\%%p.dll" "open.mp\plugins\" >nul 2>nul
)
echo [OK] Plugins deployed

REM === COPY CEF SCRIPTFILES ===
if exist "scriptfiles\cef" (
    if not exist "open.mp\scriptfiles\cef" mkdir "open.mp\scriptfiles\cef"
    xcopy /E /I /Y /Q "scriptfiles\cef" "open.mp\scriptfiles\cef" >nul 2>nul
)
echo [OK] CEF resources deployed

REM === COPY STATIC SCRIPTFILES (only if open.mp dirs don't exist) ===
for %%d in (Organizacije Dileri Firme AutoSaloni Garages Kontejneri Kapije Parkinzi Pumpe Bankomati Events GPS Granice Furniture Aktori Poslovi Radars Ulice Zones) do (
    if not exist "open.mp\scriptfiles\%%d" (
        if exist "scriptfiles\%%d" (
            xcopy /E /I /Y /Q "scriptfiles\%%d" "open.mp\scriptfiles\%%d" >nul 2>nul
        )
    )
)

REM === ENSURE RUNTIME DIRS EXIST ===
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Streljane Logovi Inventory) do (
    if not exist "open.mp\scriptfiles\%%d" mkdir "open.mp\scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles synced

REM === VERIFY ===
echo.
echo ============================================
echo   Ready! Starting open.mp server...
echo ============================================
echo.
cd /d "%~dp0open.mp"
omp-server.exe
