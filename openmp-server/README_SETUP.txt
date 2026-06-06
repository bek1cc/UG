================================================================
  UNICATE GAMING - open.mp Server Setup
================================================================

OVO JE OPEN.MP SERVER SA CEF PODRŠKOM

ŠTO IDE NA FTP (CIJELI OVAJ FOLDER):
=====================================
- omp-server          (Linux binary)
- config.json         (open.mp konfiguracija)
- server.cfg          (legacy config - open.mp čita i ovo)
- components/         (open.mp ugrađene komponente + Cef.so)
- gamemodes/          (fg-ogc.amx je ovdje)
- filterscripts/      (anims.amx je ovdje)
- plugins/            (legacy SA-MP plugini: crashdetect, streamer, sscanf, MapAndreas, SKY)
- scriptfiles/        (sve igrične podatke - korisnici, firme itd.)
  - scriptfiles/cef/  (CEF web resursi - tablet, inventory, laptop HTML/CSS/JS)
- models/             (custom modeli)
- npcmodes/           (NPC skripte)
- qawno/              (Pawn kompajler za open.mp)

KONFIGURACIJA:
==============
- Glavna konfiguracija: config.json
- Server IP: 135.125.156.197
- Port: 7777
- Max igrača: 200
- Gamemode: fg-ogc
- CEF: UKLJUČEN (omp-cef v1.2.0 component)

CEF PLUGIN:
===========
- Cef.so je u components/ folderu (pre-built binary)
- CEF koristi UDP port: 7779 (server_port + 2) - OBAVEZNO OTVORITI!
- cef.inc je u qawno/include/ folderu
- Web resursi su u scriptfiles/cef/tablet/, scriptfiles/cef/inventory/, scriptfiles/cef/laptop/

KLIJENT STRANA (ZA IGRAČE):
============================
Igrači moraju instalirati:
1. cef.asi - staviti u GTA San Andreas root folder
2. client-files/ - cef/ folder sa libcef.dll itd. u GTA San Andreas root
3. ASI Loader (ako nemaju)
Preuzmi sa: https://github.com/aurora-mp/omp-cef/releases/tag/v1.2.0
- cef.asi (94 KB)
- client-files-v1.2.0.zip (~296 MB - Chromium runtime)

POKRETANJE:
===========
Na FTP serveru, pokreni:
  ./omp-server

FIREWALL PORTOVI:
=================
- UDP 7777 - game server
- UDP 7778 - server query (automatski)
- UDP 7779 - CEF komunikacija (OBAVEZNO!)
