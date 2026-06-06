# Unicate Gaming Launcher

Profesionalni launcher za SA-MP server **Unicate Gaming**.

## Svi igraci MORAJU instalirati ovaj launcher da bi igrali na serveru!

## Funkcionalnosti

- **Server Status** - Automatska provjera da li je server online/offline
- **Player Count** - Prikaz broja igraca na serveru u realnom vremenu
- **Auto-detect GTA SA** - Automatski pronalazi GTA San Andreas instalaciju
- **CEF Plugin Check** - Provjerava da li je CEF plugin instaliran (za Tablet/Inventar/Laptop UI)
- **One-click Launch** - Stisni "LAUNCH" i igra se pokrece sa konekcijom na server
- **Novosti** - Prikaz najnovijih vijesti sa servera
- **Brzi linkovi** - Website, Discord, Forum

## Kompajliranje u .exe (za distribuciju igracima)

### Potrebno:
- Python 3.8+ (https://python.org)
- pip (dolazi sa Python-om)

### Automatski build:
```
build.bat
```

### Rucno:
```bash
pip install pyinstaller pillow requests
pyinstaller --onefile --windowed --name="Unicate Gaming" launcher.py
```

Nakon kompajliranja, .exe fajl ce biti u `dist/` folderu.

## Testiranje (bez kompajliranja):
```
run.bat
```
ili:
```bash
pip install pillow requests
python launcher.py
```

## Konfiguracija

Promijeni u `launcher.py`:
- `SERVER_IP` - IP adresa tvog servera
- `SERVER_PORT` - Port servera
- `WEBSITE_URL` - Link ka website-u
- `DISCORD_URL` - Link ka Discord serveru

## Distribucija igracima

1. Kompajliraj launcher sa `build.bat`
2. Daj igracima `Unicate Gaming.exe` iz `dist/` foldera
3. Igraci moraju imati GTA San Andreas + SA-MP client instalirane
4. Launcher automatski detektuje instalaciju

## Struktura

```
launcher/
  launcher.py    - Glavni kod launchera
  build.bat      - Skripta za kompajliranje u .exe
  run.bat        - Skripta za testiranje bez kompajliranja
  README.md      - Ovaj fajl
```
