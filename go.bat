@echo off
echo ============================================
echo   UNICATE GAMING - Compile + Deploy + Start
echo ============================================
echo.

REM Kill any running server
taskkill /f /im omp-server.exe >nul 2>nul
timeout /t 2 /nobreak >nul
echo [OK] Server stopped (if was running)
echo.

REM === DELETE OLD .amx FILES ===
echo Cleaning old .amx files...
del /f /q "gamemodes\fg-ogc.amx" 2>nul
del /f /q "fg-ogc.amx" 2>nul
del /f /q "open.mp\gamemodes\fg-ogc.amx" 2>nul
echo [OK] Old files cleaned
echo.

REM === COMPILE ===
echo Compiling fg-ogc.pwn ...
echo.
pawno\pawncc.exe gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -ipawno\include -;+

echo.
echo Looking for compiled .amx ...
set AMX_SOURCE=
if exist "gamemodes\fg-ogc.amx" (
    echo [OK] fg-ogc.amx found in gamemodes\
    set AMX_SOURCE=gamemodes\fg-ogc.amx
) else if exist "fg-ogc.amx" (
    echo [OK] fg-ogc.amx found in root - moving to gamemodes\
    move /Y "fg-ogc.amx" "gamemodes\fg-ogc.amx"
    set AMX_SOURCE=gamemodes\fg-ogc.amx
) else (
    echo [ERROR] fg-ogc.amx not found! Check compiler output above.
    pause
    exit /b 1
)

REM === DEPLOY ===
echo.
echo ============================================
echo   Deploying to open.mp server...
echo ============================================
echo.

if not exist "open.mp\gamemodes" mkdir "open.mp\gamemodes"
if not exist "open.mp\plugins" mkdir "open.mp\plugins"
if not exist "open.mp\scriptfiles" mkdir "open.mp\scriptfiles"
if not exist "open.mp\components" mkdir "open.mp\components"
if not exist "open.mp\scriptfiles\cef" mkdir "open.mp\scriptfiles\cef"
if not exist "open.mp\scriptfiles\cef\tablet" mkdir "open.mp\scriptfiles\cef\tablet"
if not exist "open.mp\scriptfiles\cef\inventory" mkdir "open.mp\scriptfiles\cef\inventory"
if not exist "open.mp\scriptfiles\cef\laptop" mkdir "open.mp\scriptfiles\cef\laptop"
if not exist "open.mp\scriptfiles\cef\loading" mkdir "open.mp\scriptfiles\cef\loading"
if not exist "open.mp\scriptfiles\cef\portal" mkdir "open.mp\scriptfiles\cef\portal"
if not exist "open.mp\scriptfiles\cef\phone" mkdir "open.mp\scriptfiles\cef\phone"
if not exist "open.mp\scriptfiles\cef\amenu" mkdir "open.mp\scriptfiles\cef\amenu"

REM Copy gamemode
echo Copying gamemode...
copy /Y "gamemodes\fg-ogc.amx" "open.mp\gamemodes\fg-ogc.amx"
if errorlevel 1 (
    echo [ERROR] Failed to copy fg-ogc.amx!
    pause
    exit /b 1
)

REM Copy plugins
if exist "plugins\crashdetect.dll" copy /Y "plugins\crashdetect.dll" "open.mp\plugins\" >nul 2>nul
if exist "plugins\streamer.dll" copy /Y "plugins\streamer.dll" "open.mp\plugins\" >nul 2>nul
if exist "plugins\sscanf.dll" copy /Y "plugins\sscanf.dll" "open.mp\plugins\" >nul 2>nul
if exist "plugins\iTD.dll" copy /Y "plugins\iTD.dll" "open.mp\plugins\" >nul 2>nul
if exist "plugins\MapAndreas.dll" copy /Y "plugins\MapAndreas.dll" "open.mp\plugins\" >nul 2>nul
echo [OK] Plugins copied

REM Copy CEF server component
if exist "open.mp\components\Cef.dll" (
    echo [OK] Cef.dll already in components
) else (
    echo [!!!] Cef.dll NOT FOUND in open.mp\components\ - CEF WILL NOT WORK!
    echo      Download from: https://github.com/aurora-mp/omp-cef/releases
    echo      Place Cef.dll in: D:\UG\open.mp\components\
)

REM Copy CEF web resources first (these are static and should always be updated)
if exist "scriptfiles\cef" (
    xcopy /E /I /Y /Q "scriptfiles\cef" "open.mp\scriptfiles\cef" >nul 2>nul
)

REM Copy scriptfiles (STATIC ONLY - do NOT overwrite runtime player data!)
if exist "scriptfiles\Logovi" mkdir "open.mp\scriptfiles\Logovi" 2>nul
REM Ensure runtime data directories exist but do NOT copy/overwrite their contents
for %%d in (Korisnici Igraci Admini Admins Helperi Multiacc Organizacije Dileri Firme AutoSaloni Garages Kontejneri Kapije Imanja Parkinzi Pumpe Streljane Bankomati Events GPS Granice Inventory Kladionica Novcanici Tagovi Furniture Aktori) do (
    if not exist "open.mp\scriptfiles\%%d" mkdir "open.mp\scriptfiles\%%d" 2>nul
)
echo [OK] Scriptfiles + CEF resources copied (runtime data preserved)

REM === WRITE server.cfg (open.mp reads gamemode0 from here) ===
echo Writing server.cfg...
(
echo gamemode0 fg-ogc 1
echo plugins crashdetect streamer sscanf iTD MapAndreas
echo port 7777
echo maxplayers 50
) > "open.mp\server.cfg"
echo [OK] server.cfg written

REM === WRITE config.json (for other open.mp settings, NOT gamemode) ===
echo Writing config.json...
(
echo {
echo     "artwork": {
echo         "enable": true,
echo         "model_store": "artwork"
echo     },
echo     "game": {
echo         "chat_input_filter": true,
echo         "entry_flags": 0,
echo         "exit_flags": 1,
echo         "lag_compensation_mode": 1,
echo         "map": "Unicate Gaming RPG",
echo         "mode": "fg-ogc",
echo         "language": "Balkan",
echo         "port": 7777,
echo         "sleep": 5.0,
echo         "time": 12,
echo         "weather": 10
echo     },
echo     "logging": {
echo         "enable": true,
echo         "file": "log.txt",
echo         "format": "[%%timestamp%%] [%%level%%] %%message%%",
echo         "level": "info",
echo         "log_chat": true,
echo         "log_connections": true,
echo         "log_deaths": true,
echo         "log_queries": true,
echo         "timestamp_format": "%%Y-%%m-%%dT%%H:%%M:%%S%%z"
echo     },
echo     "network": {
echo         "acks_limit": 3000,
echo         "aiming_sync_rate": 30,
echo         "bullet_sync_rate": 30,
echo         "carry_on_flag": false,
echo         "cookie_reseed_time": 30000,
echo         "host": "127.0.0.1",
echo         "in_car_sync_rate": 30,
echo         "lan_mode": false,
echo         "logging": true,
echo         "max_players": 50,
echo         "messages_limit": 500,
echo         "minimum_connection_time": 0,
echo         "mtu": 576,
echo         "multiplier": 10,
echo         "onfoot_sync_rate": 30,
echo         "player_marker_mode": 1,
echo         "player_timeout": 10000,
echo         "port": 7777,
echo         "public_addr": "",
echo         "rcon": true,
echo         "stream_distance": 200.0,
echo         "stream_rate": 1000,
echo         "timestamp_duration": 30000,
echo         "unoccupied_sync_rate": 30,
echo         "use_cookies": true
echo     },
echo     "pawn": {
echo         "scripts": ["fg-ogc"],
echo         "legacy_plugins": ["crashdetect", "streamer", "sscanf", "iTD", "MapAndreas"]
echo     },
echo     "rcon": {
echo         "allow_teleport": false,
echo         "enable": true,
echo         "password": "bugarinbog"
echo     }
echo }
) > "open.mp\config.json"
echo [OK] config.json written

REM === VERIFY ===
echo.
echo ============================================
echo   VERIFICATION
echo ============================================
echo.
echo --- Source .amx (should be NEW timestamp) ---
dir "gamemodes\fg-ogc.amx" 2>nul
echo.
echo --- Dest .amx (should match source) ---
dir "open.mp\gamemodes\fg-ogc.amx" 2>nul
echo.
echo --- server.cfg (gamemode + plugins) ---
type "open.mp\server.cfg"
echo.
echo --- CEF Check ---
if exist "open.mp\components\Cef.dll" (
    echo [OK] Cef.dll found - CEF should work!
) else (
    echo [!!!] Cef.dll NOT FOUND - LOADING SCREEN AND PORTAL WILL NOT WORK!
    echo      Download from: https://github.com/aurora-mp/omp-cef/releases
    echo      Place Cef.dll in: D:\UG\open.mp\components\
)

if not exist "open.mp\gamemodes\fg-ogc.amx" (
    echo [ERROR] fg-ogc.amx NOT FOUND in open.mp\gamemodes\!
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Starting server...
echo ============================================
echo.
cd /d D:\UG\open.mp
omp-server.exe
