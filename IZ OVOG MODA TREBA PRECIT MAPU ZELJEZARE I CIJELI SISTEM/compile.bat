@echo off
cd /d "D:\Unicate Gaming\pawno"
pawncc.exe "..\gamemodes\fg-ogc.pwn" -r -d3 -Z+
copy /Y "fg-ogc.amx" "..\gamemodes\fg-ogc.amx"
pause