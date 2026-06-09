@echo off
echo ============================================
echo   UG CEF - Setup Hosts File Entry
echo   Dodaje "cef" hostname u Windows hosts file
echo   Ovo je POTREBNO da CEF loading screen radi!
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

:: Check if entry already exists
findstr /C:"cef" %SystemRoot%\System32\drivers\etc\hosts >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] "cef" hostname vec postoji u hosts file-u!
    echo.
    findstr /C:"cef" %SystemRoot%\System32\drivers\etc\hosts
    echo.
    pause
    exit /b 0
)

:: Add the entry
echo [1/2] Dodajem "127.0.0.1   cef" u hosts file...
echo. >> %SystemRoot%\System32\drivers\etc\hosts
echo 127.0.0.1   cef >> %SystemRoot%\System32\drivers\etc\hosts

if %errorLevel% equ 0 (
    echo [OK] Hosts file unos uspjesno dodan!
) else (
    echo [ERROR] Nije moguce dodati unos u hosts file!
    pause
    exit /b 1
)

:: Verify
echo.
echo [2/2] Provjera...
findstr /C:"cef" %SystemRoot%\System32\drivers\etc\hosts >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] "cef" hostname je uspjesno registrovan!
    echo.
    echo Sada CEF loading screen treba da radi.
    echo Pokreni server i testiraj!
) else (
    echo [ERROR] Provjera nije uspjela. Pokusaj rucno dodati:
    echo         127.0.0.1   cef
    echo         u %SystemRoot%\System32\drivers\etc\hosts
)

echo.
echo ============================================
echo   Gotovo!
echo ============================================
echo.
pause
