#!/bin/bash
# ============================================================
#  UNICATE GAMING OGC - Linux Deploy Script
#  SA-MP / open.mp server setup za Linux host
# ============================================================

set -e

# ---- KONFIGURACIJA ----
SERVER_DIR="/home/samp/server"       # Glavni server direktorij (promijeni po potrebi)
OMP_VERSION="0.8.0"                  # open.mp verzija

# Boje za output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}"
echo "============================================"
echo "  UNICATE GAMING OGC - Linux Deploy"
echo "  open.mp Server Setup"
echo "============================================"
echo -e "${NC}"

# ---- 1. PROVJERA PRAVA ----
echo -e "${YELLOW}[1/8] Provjera prava...${NC}"
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}Pokreni skriptu kao root ili sa sudo!${NC}"
    exit 1
fi

# ---- 2. INSTALACIJA ZAVISNOSTI ----
echo -e "${YELLOW}[2/8] Instalacija zavisnosti...${NC}"
if command -v apt-get &> /dev/null; then
    apt-get update -qq
    apt-get install -y -qq wget curl unzip libmysqlclient-dev libstdc++6 libc6 screen 2>/dev/null || true
elif command -v yum &> /dev/null; then
    yum install -y wget curl unzip mysql-devel libstdc++ glibc screen 2>/dev/null || true
elif command -v dnf &> /dev/null; then
    dnf install -y wget curl unzip mysql-devel libstdc++ glibc screen 2>/dev/null || true
fi
echo -e "${GREEN}Zavisnosti instalirane.${NC}"

# ---- 3. KREIRANJE DIREKTORIJA ----
echo -e "${YELLOW}[3/8] Kreiranje server direktorija...${NC}"
mkdir -p "$SERVER_DIR"
cd "$SERVER_DIR"

# ---- 4. DOWNLOAD open.mp SERVER ----
echo -e "${YELLOW}[4/8] Download open.mp servera...${NC}"
if [ ! -f "$SERVER_DIR/omp-server" ]; then
    echo -e "${YELLOW}Skidam open.mp v${OMP_VERSION} za Linux...${NC}"
    wget -q "https://github.com/openmultiplayer/open.mp/releases/download/v${OMP_VERSION}/omp-linux-x86_64.tar.gz" -O /tmp/omp.tar.gz 2>/dev/null || \
    wget -q "https://github.com/openmultiplayer/open.mp/releases/download/v${OMP_VERSION}/omp-linux-x86.tar.gz" -O /tmp/omp.tar.gz 2>/dev/null || {
        echo -e "${RED}Ne mogu skinuti open.mp server!${NC}"
        echo -e "${YELLOW}Skinite rucno sa: https://github.com/openmultiplayer/open.mp/releases${NC}"
        echo -e "${YELLOW}I raspakujte u: $SERVER_DIR${NC}"
        echo -e "${YELLOW}Nastavljam dalje...${NC}"
    }
    if [ -f /tmp/omp.tar.gz ]; then
        tar -xzf /tmp/omp.tar.gz -C "$SERVER_DIR" --strip-components=1 2>/dev/null || true
        rm -f /tmp/omp.tar.gz
        chmod +x "$SERVER_DIR/omp-server" 2>/dev/null || true
        echo -e "${GREEN}open.mp server skinut.${NC}"
    fi
else
    echo -e "${GREEN}open.mp server vec postoji.${NC}"
fi

# ---- 5. KOPIRANJE SERVER FAJLOVA ----
echo -e "${YELLOW}[5/8] Kopiranje server fajlova...${NC}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Kopiraj konfiguraciju
cp "$SCRIPT_DIR/open.mp/config.json" "$SERVER_DIR/" 2>/dev/null || true

# Kopiraj komponente (samo .so - Linux)
mkdir -p "$SERVER_DIR/components"
cp "$SCRIPT_DIR/open.mp/components/"*.so "$SERVER_DIR/components/" 2>/dev/null || true

# Kopiraj pluginove (samo .so - Linux)
mkdir -p "$SERVER_DIR/plugins"
cp "$SCRIPT_DIR/plugins/"*.so "$SERVER_DIR/plugins/" 2>/dev/null || true

# Kopiraj Pawn includes
mkdir -p "$SERVER_DIR/qawno/include"
cp "$SCRIPT_DIR/open.mp/qawno/include/"* "$SERVER_DIR/qawno/include/" 2>/dev/null || true

# Kopiraj gamemode
mkdir -p "$SERVER_DIR/gamemodes"
cp "$SCRIPT_DIR/gamemodes/fg-ogc.pwn" "$SERVER_DIR/gamemodes/"
cp -r "$SCRIPT_DIR/gamemodes/systems" "$SERVER_DIR/gamemodes/"
cp -r "$SCRIPT_DIR/gamemodes/maps" "$SERVER_DIR/gamemodes/"

# Kopiraj filterscripts
mkdir -p "$SERVER_DIR/filterscripts"
cp "$SCRIPT_DIR/open.mp/filterscripts/"* "$SERVER_DIR/filterscripts/" 2>/dev/null || true

# Kopiraj scriptfiles (CEF + data)
cp -r "$SCRIPT_DIR/scriptfiles" "$SERVER_DIR/"

echo -e "${GREEN}Server fajlovi kopirani.${NC}"

# ---- 6. KOMPILACIJA GAMEMODEA ----
echo -e "${YELLOW}[6/8] Kompilacija gamemodea...${NC}"
if [ -f "$SERVER_DIR/qawno/pawncc" ]; then
    cd "$SERVER_DIR/gamemodes"
    "$SERVER_DIR/qawno/pawncc" fg-ogc.pwn -;+ -d3
    if [ -f "$SERVER_DIR/gamemodes/fg-ogc.amx" ]; then
        echo -e "${GREEN}Gamemode uspjesno kompajliran!${NC}"
    else
        echo -e "${RED}Greska pri kompilaciji! Provjeri logove.${NC}"
    fi
else
    echo -e "${YELLOW}Pawn kompajler nije pronaden u serveru.${NC}"
    echo -e "${YELLOW}Moras kompajlirati fg-ogc.pwn na Windowsu i prenijeti .amx fajl.${NC}"
    echo -e "${YELLOW}Kopiraj fg-ogc.amx u $SERVER_DIR/gamemodes/${NC}"
fi

# ---- 7. MYSQL SETUP ----
echo -e "${YELLOW}[7/8] MySQL konfiguracija...${NC}"
if command -v mysql &> /dev/null; then
    echo -e "${GREEN}MySQL je instaliran.${NC}"
    echo -e "${CYAN}Provjeri MySQL konekciju u gamemode skripti (fg-ogc.pwn)${NC}"
else
    echo -e "${YELLOW}MySQL nije pronaden. Instaliraj MySQL/MariaDB:${NC}"
    echo -e "  Ubuntu/Debian: sudo apt install mysql-server"
    echo -e "  CentOS/RHEL:   sudo yum install mysql-server"
fi

# ---- 8. PRAVA I FINALIZACIJA ----
echo -e "${YELLOW}[8/8] Postavljanje prava...${NC}"
chmod +x "$SERVER_DIR/omp-server" 2>/dev/null || true
chmod -R 755 "$SERVER_DIR/plugins/" 2>/dev/null || true
chmod -R 755 "$SERVER_DIR/components/" 2>/dev/null || true
chmod -R 777 "$SERVER_DIR/scriptfiles/" 2>/dev/null || true

echo -e "${GREEN}"
echo "============================================"
echo "  DEPLOY ZAVRSEN!"
echo "============================================"
echo ""
echo "  Server direktorij: $SERVER_DIR"
echo ""
echo "  POKRETANJE SERVERA:"
echo "    cd $SERVER_DIR"
echo "    ./omp-server"
echo ""
echo "  ILI SA SCREEN (preporuceno):"
echo "    screen -AmdS unicate ./omp-server"
echo "    screen -r unicate  (za prikaz konzole)"
echo "    Ctrl+A, D  (za detach)"
echo ""
echo "  VAZNO:"
echo "    1. Provjeri config.json (port, IP, rcon pass)"
echo "    2. Kompajliraj fg-ogc.pwn ako nisi"
echo "    3. Podesi MySQL konekciju u skripti"
echo "    4. Otvori port 7777 na firewall-u:"
echo "       sudo ufw allow 7777"
echo "       sudo ufw allow 7777/udp"
echo "============================================"
echo -e "${NC}"
