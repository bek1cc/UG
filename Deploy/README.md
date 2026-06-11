# UNICATE GAMING OGC - Linux Deploy

Kompaktan Deploy folder - SAMO prebaci na FTP i pokreni.

## Struktura

```
Deploy/
├── deploy.sh              # Pokreni ovo na Linuxu
├── open.mp/               # CIJELI server (komponente, includes, scriptfiles, config)
│   ├── components/        # 46 komponenti (.so Linux verzije)
│   ├── filterscripts/     # Filter skripte
│   ├── npcmodes/          # NPC modovi
│   ├── plugins/           # Plugini (.so)
│   ├── qawno/include/     # 139 Pawn includes
│   ├── scriptfiles/       # SVI podaci (CEF, firme, organizacije itd)
│   ├── config.json        # Server konfiguracija (IP: 217.156.22.164:7774)
│   └── server.cfg         # Legacy konfiguracija
├── gamemodes/             # fg-ogc.pwn + systems + maps
├── extra-plugins/         # Dodatni Linux .so plugini
├── launcher/              # Windows launcher (za igrace)
└── scriptfiles/           # Extra scriptfiles
```

## Brzi Deploy (3 koraka)

### 1. Prebaci na FTP
Cijeli `Deploy/` folder prebaci u `/home/samp/server/`

### 2. SSH na server
```bash
cd /home/samp/server
chmod +x deploy.sh
sudo ./deploy.sh
```

### 3. Pokreni server
```bash
export DISPLAY=:99
cd /home/samp/server
./omp-server
```

## Sta FALI (moras dodati rucno)

1. **fg-ogc.amx** - Kompajliraj `fg-ogc.pwn` na Windowsu, prebaci .amx u `gamemodes/`
2. **omp-server** - Skini sa https://github.com/openmultiplayer/open.mp/releases

## CEF Fix

CEF zahtijeva Xvfb na Linuxu. deploy.sh ga automatski instalira i pokrece.
Ako CEF ne radi, provjeri:
```bash
echo $DISPLAY   # Mora biti :99
ps aux | grep Xvfb   # Mora biti pokrenut
```

## Portovi

| Port | Protokol | Opis |
|------|----------|------|
| 7774 | UDP | SA-MP game server |
| 7774 | TCP | RCON |
| 3306 | TCP | MySQL |
