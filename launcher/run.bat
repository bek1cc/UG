@echo off
title Unicate Gaming Launcher - Pokreni bez kompajliranja
echo Pokrecem Unicate Gaming Launcher...
python launcher.py
if %errorlevel% neq 0 (
    echo.
    echo Greska! Provjeri da li je Python instaliran.
    echo Pokreni: pip install pillow requests
    pause
)
