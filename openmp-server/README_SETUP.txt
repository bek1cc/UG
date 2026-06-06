================================================================
  UNICATE GAMING - open.mp Server Setup
================================================================

OVO JE OPEN.MP SERVER - ZAMJENA ZA SA-MP SERVER

STO IDE NA FTP (CIJELI OVAJ FOLDER):
=====================================
- omp-server          (Linux binary)
- config.json         (open.mp konfiguracija)
- server.cfg          (legacy config - open.mp cita i ovo)
- components/         (open.mp ugradjene komponente)
- gamemodes/          (fg-ogc.amx je ovdje)
- filterscripts/      (anims.amx je ovdje)
- plugins/            (legacy SA-MP plugini: crashdetect, streamer, sscanf, MapAndreas, SKY)
- scriptfiles/        (sve igricne podatke - korisnici, firme itd.)
- models/             (custom modeli)
- npcmodes/           (NPC skripte)

KONFIGURACIJA:
==============
- Glavna konfiguracija: config.json
- Server IP: 135.125.156.197
- Port: 7777
- Max igaca: 200
- Gamemode: fg-ogc
- CEF: ONEMOGUCEN (dok se ne kompajlira omp-cef plugin)

POKRETANJE:
===========
Na FTP serveru, pokreni:
  ./omp-server

CEF PLUGIN (ZA POSLIJE):
========================
Da bi CEF radio, treba:
1. Kompajlirati omp-cef (https://github.com/aurora-mp/omp-cef)
2. Staviti u components/ folder
3. Otkomentirati #define USE_CEF u gamemode
4. Rekompajlirati gamemode

ZA SADA:
========
Server radi bez CEF-a. Sve standardne stvari rade normalno.
CEF stvari (tablet, inventar UI, laptop UI) nece raditi dok se
ne doda omp-cef plugin.
