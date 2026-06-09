# Unicate Gaming Launcher v3.8

Electron-based launcher za SA-MP 0.3.7-R4/R5 server "Unicate Gaming RPG".

## Funkcije

- Server status query (UDP) sa auto-refresh
- Auto-instalacija SA-MP clienta, ASI Loadera, CEF plugina i Chromium Runtime-a
- UG Splash screen (custom samp.dll sa UG bitmapom)
- CEF Loading Screen (25s sa muzikom i tipovima)
- CEF Portal (Login/Register forme)
- Produkcijski i lokalni test mod
- CEF toggle (za lokalni test bez CEF-a)
- Pre-launch crash fix (zombie process, corrupt gta_sa.set, compatibility mode)

## Pokretanje (development)

```bash
npm install
npm start
```

## Build (portable EXE)

```bash
npm run build
```

Output: `dist/UnicateGamingLauncher.exe`

## UG Splash Setup

Da bi UG splash screen radio, trebas kreirati `ug_samp.dll` - patchovanu samp.dll sa UG splash bitmapom:

### Metoda 1: Python patcher (preporuceno)

```bash
cd launcher/ug_launcher
python patch_splash.py "C:\path\to\GTA\samp.dll" "..\ug_splash.bmp" "..\electron-launcher\ug_samp.dll"
```

### Metoda 2: Resource Hacker (Windows)

1. Preuzmi [Resource Hacker](http://www.angusj.com/resourcehacker/)
2. Otvori `samp.dll` u Resource Hacker-u
3. Pronadji `Bitmap/128/1041`
4. Replace Resource sa `ug_splash.bmp`
5. Save As `ug_samp.dll`
6. Stavi `ug_samp.dll` pored launchera

Launcher ce automatski instalirati `ug_samp.dll` kao `samp.dll` u GTA folder kad pokrenes igru.

## CEF Content

CEF loading screen i portal su u `cef_content/` folderu:
- `cef_content/loading/` - 25s loading screen (index.html, logo.png, song.mp3)
- `cef_content/portal/` - Portal sa Login/Register (index.html, logo.png, bg_city.png)

Launcher automatski kopira ove fajlove u `GTA/cef/loading/` i `GTA/cef/portal/` prije pokretanja igre.

## Struktura

```
electron-launcher/
  src/
    main.js         - Electron main process
    preload.js      - Context bridge (IPC)
    renderer.js     - Frontend logika
    index.html      - UI layout
    styles.css      - Next-level dark theme CSS
    ug_logo.png     - 500x500 PNG logo
    ug_icon.ico     - Windows icon
    bg_gta.png      - Background image
  cef_content/
    loading/        - CEF loading screen
    portal/         - CEF portal
  package.json
  run.bat
  ug_splash.bmp     - UG splash bitmap (za patch)
  ug_splash.png     - UG splash preview
```
