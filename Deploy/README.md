# UNICATE GAMING OGC - Linux Deploy

Sve sto trebas za deploy SA-MP/open.mp servera na Linux host.

## Struktura

```
Deploy/
├── deploy.sh                    # Automatski deploy script
├── open.mp/
│   ├── config.json              # Server konfiguracija
│   ├── components/              # open.mp komponente (.so)
│   │   └── Cef.so              # CEF za browser u igri
│   ├── filterscripts/           # Filter skripte
│   │   ├── npc_trains.amx
│   │   └── npc_trains.pwn
│   └── qawno/include/           # Pawn include fajlovi (139)
├── gamemodes/
│   ├── fg-ogc.pwn              # Glavni gamemode (5.3MB)
│   ├── systems/                # Sistemi (kliziste, plastika, zeljezara)
│   └── maps/                   # Map includeovi
├── plugins/                    # Linux plugini (.so)
│   ├── crashdetect.so
│   ├── streamer.so
│   ├── sscanf.so
│   ├── mysql.so
│   ├── MapAndreas.so
│   └── SKY.so
└── scriptfiles/
    ├── cef/                    # CEF web stranice
    │   ├── portal/            # Login/Register portal
    │   ├── case/              # Case opening
    │   ├── amenu/             # Admin meni
    │   ├── tablet/            # Tablet browser
    │   ├── inventory/         # Inventar
    │   ├── laptop/            # Laptop
    │   └── phone/             # Telefon
    ├── Events/                # Event podaci
    ├── Firme/                 # Firma podaci
    ├── Organizacije/          # Org podaci
    ├── Bankomati/             # Bankomat podaci
    └── ...                    # Ostali podaci
```

## Brzi Deploy

```bash
# 1. Upload Deploy folder na Linux host
scp -r Deploy/ root@tvoj-server:/tmp/Deploy/

# 2. Pokreni deploy script
ssh root@tvoj-server
cd /tmp/Deploy
chmod +x deploy.sh
sudo ./deploy.sh
```

## Rucni Deploy

```bash
# 1. Download open.mp server
wget https://github.com/openmultiplayer/open.mp/releases/latest/download/omp-linux-x86_64.tar.gz
tar -xzf omp-linux-x86_64.tar.gz -C /home/samp/server --strip-components=1

# 2. Kopiraj fajlove iz Deploy/ u server direktorij
cp -r Deploy/open.mp/* /home/samp/server/
cp -r Deploy/gamemodes/ /home/samp/server/
cp -r Deploy/plugins/ /home/samp/server/
cp -r Deploy/scriptfiles/ /home/samp/server/

# 3. Kompajliraj gamemode (ako imas pawncc)
cd /home/samp/server/gamemodes
./qawno/pawncc fg-ogc.pwn -;+ -d3

# 4. Pokreni server
cd /home/samp/server
chmod +x omp-server
screen -AmdS unicate ./omp-server
```

## Sta trebas podesiti

1. **config.json** - Promijeni IP, port, rcon password
2. **MySQL** - Podesi konekciju u fg-ogc.pwn
3. **Firewall** - Otvori port 7777 (TCP + UDP)
4. **Gamemode** - Kompajliraj na Windowsu ako nemas pawncc na Linuxu

## Portovi

| Port | Protokol | Opis |
|------|----------|------|
| 7777 | UDP | SA-MP game server |
| 7777 | TCP | RCON |
| 3306 | TCP | MySQL (ako je lokalni) |

## Napomene

- **fg-ogc.amx** nije ukljucen - moras kompajlirati fg-ogc.pwn
- Svi plugini su Linux (.so) verzije
- Cef.so komponenta je potrebna za CEF browser
- scriptfiles/Igraci/ i scriptfiles/Korisnici/ ce se popuniti automatski kad igraci udju
- Provjeri da je MySQL pokrenut prije startanja servera
