@echo off
title SA-MP 0.3.DL Server Setup - Unicate Gaming RPG
echo ============================================
echo   SA-MP 0.3.DL Server Setup
echo   Unicate Gaming RPG
echo ============================================
echo.

:: Provjeri da li samp-server.exe vec postoji
if exist "samp-server.exe" (
    echo [INFO] samp-server.exe vec postoji! Preskacem download.
    echo [INFO] Ako zelis reinstalirati, obrisi samp-server.exe i pokreni opet.
    goto :start_server
)

:: Provjeri da li imamo PowerShell
where powershell >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [GRESKA] PowerShell nije pronadjen. Ne mogu skinuti server.
    echo [INFO] Rucno skini SA-MP 0.3.DL server sa: https://sa-mp.com/download.php
    echo [INFO] Raspakuj u ovaj folder i pokreni samp-server.exe
    pause
    exit /b 1
)

echo [1/3] Skidam SA-MP 0.3.DL R1 server za Windows...
powershell -Command "& {Invoke-WebRequest -Uri 'https://files.sa-mp.com/samp037_svr_R2-1-1.zip' -OutFile 'samp_server.zip'}" 2>nul

:: Ako sa-mp.com ne radi, pokusaj alternativni link
if not exist "samp_server.zip" (
    echo [INFO] Primarni link ne radi, pokusavam alternativni...
    echo [INFO] Skidam sa-mp-0.3.DL-R1-server...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://files.sa-mp.com/samp03DL_svr_R1.zip' -OutFile 'samp_server.zip'}" 2>nul
)

if not exist "samp_server.zip" (
    echo [GRESKA] Ne mogu skinuti server fajl.
    echo [INFO] Rucno skini SA-MP 0.3.DL server sa:
    echo        https://sa-mp.com/download.php
    echo        Raspakuj samp03DL_svr_R1.zip i kopiraj:
    echo          - samp-server.exe
    echo          - announce.exe
    echo          - samp-npc.exe
    echo        u ovaj folder.
    pause
    exit /b 1
)

echo [2/3] Raspakujem server fajlove...
powershell -Command "& {Expand-Archive -Path 'samp_server.zip' -DestinationPath 'samp_temp' -Force}"

:: Kopiraj potrebne fajlove iz raspakovanog direktorija
echo [3/3] Kopiram server fajlove...
for /d %%d in (samp_temp\samp*) do (
    if exist "%%d\samp-server.exe" (
        copy "%%d\samp-server.exe" . >nul
        copy "%%d\announce.exe" . >nul 2>nul
        copy "%%d\samp-npc.exe" . >nul 2>nul
        if not exist "npcmodes" mkdir npcmodes
        copy "%%d\npcmodes\*.amx" "npcmodes\" >nul 2>nul
    )
)

:: Cleanup
del samp_server.zip >nul 2>nul
rmdir /s /q samp_temp >nul 2>nul

:: Provjeri
if exist "samp-server.exe" (
    echo.
    echo [USPJEH] SA-MP 0.3.DL server instaliran!
) else (
    echo.
    echo [GRESKA] Instalacija nije uspjela. Rucno skini sa https://sa-mp.com/download.php
    pause
    exit /b 1
)

:start_server
echo.
echo ============================================
echo   Pokrecem server...
echo ============================================
echo.
echo VAZNO: Server koristi SA-MP 0.3.DL
echo   - AddSimpleModel radi za custom modele
echo   - SetPlayerAttachedObject radi s custom ID
echo   - Klijenti moraju imati 0.3.DL instaliran
echo.
echo Modeli u models/ folderu:
dir /b models\*.dff models\*.txd 2>nul
echo.
echo Port: 7777 | RCON: russss
echo ============================================
echo.

start samp-server.exe
echo Server pokrenut! Pogledaj server console prozor.
pause
