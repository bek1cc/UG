@echo off
echo ============================================
echo    Unicate Gaming Launcher v3.0 - Build .exe
echo ============================================
echo.

:: Provjeri da li je Python instaliran
python --version >nul 2>&1
if errorlevel 1 (
    echo [GRESKA] Python nije instaliran!
    echo Preuzmi Python sa: https://www.python.org/downloads/
    echo Ne zaboravi ceknuti "Add Python to PATH" pri instalaciji!
    pause
    exit /b 1
)

:: Instaliraj PyInstaller i Pillow ako nisu
echo [1/4] Provjera zavisnosti...
pip show pyinstaller >nul 2>&1
if errorlevel 1 (
    echo Instaliranje PyInstaller-a...
    pip install pyinstaller
)

pip show pillow >nul 2>&1
if errorlevel 1 (
    echo Instaliranje Pillow-a...
    pip install pillow
)

:: Kompajliraj
echo.
echo [2/4] Kompajliranje Unicate Gaming.exe ...
echo Ovo moze potrajati 1-2 minute...
echo.

pyinstaller --onefile --windowed --name="Unicate Gaming" ^
    --add-data "ug_logo.png;." ^
    launcher.py

if errorlevel 1 (
    echo.
    echo [GRESKA] Kompajliranje nije uspjelo!
    pause
    exit /b 1
)

:: Ciscenje
echo.
echo [3/4] Ciscenje temp fajlova...
if exist build rmdir /s /q build
if exist "Unicate Gaming.spec" del /q "Unicate Gaming.spec"

:: Kopiraj u dist folder
echo.
echo [4/4] Kopiranje fajlova...
if not exist "dist" mkdir "dist"
copy /y "ug_logo.png" "dist\ug_logo.png" >nul

echo.
echo ============================================
echo    GOTOVO!
echo ============================================
echo.
echo .exe fajl je u: dist\Unicate Gaming.exe
echo.
echo Da bi launcher radio za igrace, napravi ovu strukturu:
echo.
echo   Unicate Gaming\
echo   +-- Unicate Gaming.exe
echo   +-- ug_logo.png
echo   +-- ug_samp.dll          (modifikovani sa UG splash bitmapom)
echo   +-- ug_splash.bmp        (splash bitmapa za samp.dll)
echo   +-- asi_files\
echo   !   +-- vorbisFile.dll
echo   !   +-- vorbisHooked.dll
echo   +-- cef_files\
echo       +-- samp_cef.dll      (CEF plugin)
echo       +-- cef\
echo           +-- libcef.dll
echo           +-- icudtl.dat
echo           +-- ... (svi Chromium fajlovi)
echo.
echo LAUNCHER CE AUTO-INSTALIRATI SVE FAJLOVE!
echo Igrac samo pokrene .exe i klikne KONEKTUJ SE.
echo.
echo Ovu cijelu mapu zipuj i distribuiraj igracima!
echo.
pause
