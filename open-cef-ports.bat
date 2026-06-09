@echo off
echo ============================================
echo   CEF Port Opener - UG Server
echo   Otvara UDP port 7779 za CEF plugin
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

echo [1/3] Otvaranje UDP porta 7779 (CEF Network Server)...
netsh advfirewall firewall add rule name="UG CEF Server (UDP 7779)" dir=in action=allow protocol=UDP localport=7779
if %errorLevel% equ 0 (
    echo [OK] UDP 7779 ulazni otvoren!
) else (
    echo [WARN] Greska pri otvaranju UDP 7779 ulaznog.
)

echo.
echo [2/3] Otvaranje UDP porta 7779 izlazni...
netsh advfirewall firewall add rule name="UG CEF Server Out (UDP 7779)" dir=out action=allow protocol=UDP localport=7779
if %errorLevel% equ 0 (
    echo [OK] UDP 7779 izlazni otvoren!
) else (
    echo [WARN] Greska pri otvaranju UDP 7779 izlaznog.
)

echo.
echo [3/3] Provera da li je pravilo dodato...
netsh advfirewall firewall show rule name="UG CEF Server (UDP 7779)" >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Firewall pravilo postoji!
) else (
    echo [ERROR] Pravilo nije pronadjeno. Nesto je poslo po zlu.
)

echo.
echo ============================================
echo   Gotovo! CEF port 7779 UDP je otvoren.
echo   Pokreni server i testiraj CEF!
echo ============================================
echo.
pause
