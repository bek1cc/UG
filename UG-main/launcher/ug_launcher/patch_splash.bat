@echo off
echo ============================================
echo    UG Splash Patcher za samp.dll
echo ============================================
echo.
echo Ova skripta zamjenjuje default SA-MP splash bitmapu
echo sa custom UG splash bitmapom (ug_splash.bmp) u samp.dll
echo koristeci Resource Hacker.
echo.

:: Provjeri da li ima Resource Hacker-a
where ResourceHacker.exe >nul 2>&1
if errorlevel 1 (
    echo [INFO] Resource Hacker nije pronadjen u PATH.
    echo.
    echo Preuzmi Resource Hacker sa:
    echo   http://www.angusj.com/resourcehacker/
    echo.
    echo Ekstraktuj ResourceHacker.exe u ovaj folder ili ga dodaj u PATH.
    echo.
    echo ALTERNATIVA: Ako imas vec patchovan ug_samp.dll sa UG splash bitmapom,
    echo samo ga imenuj "ug_samp.dll" i stavi pored launchera.
    echo Launcher ce ga automatski kopirati kao samp.dll u GTA folder.
    echo.
    pause
    exit /b 1
)

:: Provjeri da li ima ug_splash.bmp
if not exist "ug_splash.bmp" (
    echo [GRESKA] ug_splash.bmp nije pronadjen!
    echo Stavi UG splash bitmapu (24-bit BMP) u ovaj folder.
    pause
    exit /b 1
)

:: Provjeri da li ima originalnog samp.dll
if not exist "samp.dll" (
    echo [GRESKA] samp.dll nije pronadjen!
    echo Stavi originalni SA-MP samp.dll u ovaj folder.
    pause
    exit /b 1
)

:: Kreiraj backup
if not exist "samp.dll.original_backup" (
    copy /y "samp.dll" "samp.dll.original_backup"
    echo [OK] Backup kreiran: samp.dll.original_backup
) else (
    echo [OK] Backup vec postoji
)

:: Patch samp.dll - zamijeni bitmap resource
echo.
echo [1/2] Patchovanje samp.dll sa UG splash bitmapom...
ResourceHacker.exe -open "samp.dll" -save "ug_samp.dll" -action addoverwrite -res "ug_splash.bmp" -mask BITMAP,128,

if errorlevel 1 (
    echo.
    echo [GRESKA] Patchovanje nije uspjelo!
    echo Pokusaj rucno sa Resource Hacker-om:
    echo   1. Otvori samp.dll u Resource Hacker-u
    echo   2. Pronadji Bitmap/128
    echo   3. Replace Resource sa ug_splash.bmp
    echo   4. Save As ug_samp.dll
    pause
    exit /b 1
)

echo.
echo [2/2] Verifikacija...
if exist "ug_samp.dll" (
    echo.
    echo ============================================
    echo    GOTOVO!
    echo ============================================
    echo.
    echo ug_samp.dll je kreiran sa UG splash bitmapom!
    echo.
    echo Launcher ce automatski instalirati ug_samp.dll
    echo kao samp.dll u GTA San Andreas folder za sve igrace.
    echo.
) else (
    echo [GRESKA] ug_samp.dll nije kreiran!
)

pause
