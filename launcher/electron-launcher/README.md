@echo off
echo ========================================
echo  Unicate Gaming Launcher - Build
echo ========================================
echo.
echo Installing dependencies...
call npm install
echo.
echo Building launcher...
call npm run build
echo.
echo Done! Check the dist/ folder for the exe.
pause
