@echo off
echo ============================================
echo   UNICATE GAMING - Compile Gamemode
echo ============================================
echo.

REM Obrisi stari .amx da ne ostane zastarjeli
if exist "fg-ogc.amx" del "fg-ogc.amx"
if exist "gamemodes\fg-ogc.amx" del "gamemodes\fg-ogc.amx"
echo [OK] Stari .amx obrisan
echo.

echo Compiling fg-ogc.pwn ...
echo.

pawno\pawncc.exe gamemodes\fg-ogc.pwn -igamemodes -igamemodes\maps -igamemodes\systems -ipawno\include -;+

echo.
echo Checking for compiled file...
if exist "gamemodes\fg-ogc.amx" (
    echo [OK] Compilation successful! fg-ogc.amx created.
) else if exist "fg-ogc.amx" (
    echo [OK] Found fg-ogc.amx in current directory - moving...
    move /Y "fg-ogc.amx" "gamemodes\fg-ogc.amx"
) else (
    echo [ERROR] fg-ogc.amx not found! Check compiler output above.
    pause
    exit /b 1
)

echo.
echo ============================================
echo   Deploying to open.mp server...
echo ============================================
echo.

REM Create required folders
if not exist "open.mp\gamemodes" mkdir "open.mp\gamemodes"
if not exist "open.mp\plugins" mkdir "open.mp\plugins"
if not exist "open.mp\scriptfiles" mkdir "open.mp\scriptfiles"
if not exist "open.mp\components" mkdir "open.mp\components"
if not exist "open.mp\scriptfiles\cef" mkdir "open.mp\scriptfiles\cef"

REM Copy gamemode
copy /Y "gamemodes\fg-ogc.amx" "open.mp\gamemodes\fg-ogc.amx" >nul
echo [OK] Gamemode copied to open.mp\gamemodes\

REM Copy plugins (crashdetect first, then others)
if exist "plugins\crashdetect.dll" copy /Y "plugins\crashdetect.dll" "open.mp\plugins\" >nul
if exist "plugins\streamer.dll" copy /Y "plugins\streamer.dll" "open.mp\plugins\" >nul
if exist "plugins\sscanf.dll" copy /Y "plugins\sscanf.dll" "open.mp\plugins\" >nul
if exist "plugins\iTD.dll" copy /Y "plugins\iTD.dll" "open.mp\plugins\" >nul
if exist "plugins\MapAndreas.dll" copy /Y "plugins\MapAndreas.dll" "open.mp\plugins\" >nul
echo [OK] Plugins copied to open.mp\plugins\

REM Check for CEF server component
if exist "open.mp\components\Cef.dll" (
    echo [OK] Cef.dll already in open.mp\components\
) else (
    echo [WARN] Cef.dll NOT found in open.mp\components\
    echo        CEF will NOT work! Download from: https://github.com/aurora-mp/omp-cef/releases
    echo        Place Cef.dll in: open.mp\components\
)

REM Copy scriptfiles
echo [..] Copying scriptfiles...
xcopy /E /I /Y /Q "scriptfiles" "open.mp\scriptfiles" >nul 2>nul
echo [OK] Scriptfiles copied to open.mp\scriptfiles\

REM Write config.json directly to open.mp
echo [..] Writing config.json...
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
echo [OK] config.json written to open.mp\

echo.
echo ============================================
echo   VERIFICATION
echo ============================================
echo.
if exist "open.mp\components\Cef.dll" (
    echo [OK] Cef.dll found - CEF should work
) else (
    echo [!!!] Cef.dll NOT FOUND - LOADING SCREEN AND PORTAL WILL NOT WORK!
    echo      Download from: https://github.com/aurora-mp/omp-cef/releases
    echo      Place in: D:\UG\open.mp\components\Cef.dll
)
echo.
echo ============================================
echo   ALL DONE! Ready to start server.
echo   Run: cd open.mp ^& omp-server.exe
echo ============================================
echo.
pause
