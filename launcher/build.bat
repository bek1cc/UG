@echo off
title Build Unicate Gaming Launcher
echo ============================================
echo   UNICATE GAMING LAUNCHER - BUILD
echo ============================================
echo.
pip install pyinstaller customtkinter pillow requests
echo.
echo Kompajliram...
pyinstaller --onefile --windowed --icon=ug_icon.ico --name="UnicateGaming" UnicateGamingLauncher.py
echo.
echo Gotovo! exe je u dist/ folderu.
pause
