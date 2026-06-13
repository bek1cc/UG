@echo off
title UG Server + AI Panel
echo ============================================
echo    UG Server - Auto Start
echo    Server + AI Dev Panel
echo ============================================
echo.

:: Provjeri da li je Node.js instaliran
where node >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [GRESKA] Node.js nije instaliran!
    echo Preuzmi sa: https://nodejs.org/
    echo Pokrecem samo server bez AI panela...
    echo.
    goto :server_only
)

:: Pokreni AI backend u pozadini
echo [1/2] Pokrecem AI Dev Panel backend...
cd /d "%~dp0ai-backend"
start "UG AI Backend" /min node server.js
cd /d "%~dp0"

:: Sacekaj 2 sekunde da se backend podigne
timeout /t 2 /nobreak >nul
echo [OK] AI Dev Panel backend pokrenut na http://127.0.0.1:3777
echo.

:server_only
:: Pokreni open.mp server
echo [2/2] Pokrecem open.mp server...
cd /d "%~dp0open.mp"
"# omp-server.exe"
echo.
echo [INFO] Server je zatvoren. Zatvaram AI backend...
taskkill /fi "WINDOWTITLE eq UG AI Backend" >nul 2>&1
pause
