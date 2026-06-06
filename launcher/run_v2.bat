@echo off
title UNICATE GAMING - LAUNCHER
echo ============================================
echo   UNICATE GAMING - Launcher
echo ============================================
echo.

echo Provjeravam zavisnosti...
python -c "import customtkinter" 2>nul
if %errorlevel% neq 0 (
    echo Instaliram customtkinter...
    pip install customtkinter
)

python -c "import PIL" 2>nul
if %errorlevel% neq 0 (
    echo Instaliram Pillow...
    pip install pillow
)

python -c "import requests" 2>nul
if %errorlevel% neq 0 (
    echo Instaliram requests...
    pip install requests
)

echo.
echo Pokrecem UNICATE GAMING - LAUNCHER...
python launcher_v2.py
if %errorlevel% neq 0 (
    echo.
    echo Greska! Provjeri konzolu.
    pause
)
