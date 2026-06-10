@echo off
echo ============================================
echo   CEF Port Opener - UG Server
echo   Otvara UDP port za CEF plugin
echo ============================================
echo.

:: Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Ova skripta mora biti pokrenuta kao Administrator!
    echo.
    echo Desni klik na ovaj .bat fajl -^> "Run as administrator"
    echo.
    pause
    exit /b 1
)

:: CEF koristi server_port + 2 za KCP protokol (prenos .pak resursa)
:: Ako je server na portu 7777, CEF koristi port 7779
set CEF_PORT=7779

echo [1/4] Otvaranje UDP porta %CEF_PORT% (CEF KCP - transfer resursa)...
netsh advfirewall firewall add rule name="UG CEF KCP (UDP %CEF_PORT%)" dir=in action=allow protocol=UDP localport=%CEF_PORT%
if %errorLevel% equ 0 (
    echo [OK] UDP %CEF_PORT% ulazni otvoren!
) else (
    echo [WARN] Greska pri otvaranju UDP %CEF_PORT% ulaznog.
)

echo.
echo [2/4] Otvaranje UDP porta %CEF_PORT% izlazni...
netsh advfirewall firewall add rule name="UG CEF KCP Out (UDP %CEF_PORT%)" dir=out action=allow protocol=UDP localport=%CEF_PORT%
if %errorLevel% equ 0 (
    echo [OK] UDP %CEF_PORT% izlazni otvoren!
) else (
    echo [WARN] Greska pri otvaranju UDP %CEF_PORT% izlaznog.
)

echo.
echo [3/4] Otvaranje TCP porta %CEF_PORT% (CEF HTTP fallback)...
netsh advfirewall firewall add rule name="UG CEF HTTP (TCP %CEF_PORT%)" dir=in action=allow protocol=TCP localport=%CEF_PORT%
if %errorLevel% equ 0 (
    echo [OK] TCP %CEF_PORT% otvoren!
) else (
    echo [WARN] Greska pri otvaranju TCP %CEF_PORT%.
)

echo.
echo [4/4] Provera da li su pravila dodata...
netsh advfirewall firewall show rule name="UG CEF KCP (UDP %CEF_PORT%)" >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Firewall pravila postoje!
) else (
    echo [ERROR] Pravila nisu pronadjena.
)

echo.
echo ============================================
echo   Gotovo! CEF portovi su otvoreni.
echo.
echo   VAZNO: Hosts file unos NIJE potreban!
echo   omp-cef koristi custom scheme handler
echo   koji presrece http://cef/ URL-ove direktno
echo   u Chromium-u, bez DNS rezolucije.
echo ============================================
echo.
pause
