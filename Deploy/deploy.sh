#!/bin/bash
set -e
SERVER_DIR="/home/samp/server"
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; CYAN='\033[0;36m'; NC='\033[0m'

echo -e "${CYAN}============================================${NC}"
echo -e "${CYAN}  UNICATE GAMING OGC - Linux Deploy${NC}"
echo -e "${CYAN}============================================${NC}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 1. Server dir
echo -e "${YELLOW}[1/6] Kreiranje server direktorija...${NC}"
mkdir -p "$SERVER_DIR"

# 2. Kopiraj CIJELI open.mp folder
echo -e "${YELLOW}[2/6] Kopiranje server fajlova...${NC}"
cp -r "$SCRIPT_DIR/open.mp/"* "$SERVER_DIR/"

# 3. Kopiraj extra Linux plugins
echo -e "${YELLOW}[3/6] Kopiranje Linux pluginova...${NC}"
mkdir -p "$SERVER_DIR/plugins"
cp "$SCRIPT_DIR/extra-plugins/"*.so "$SERVER_DIR/plugins/"

# 4. Dodaj gamemode
echo -e "${YELLOW}[4/6] Kopiranje gamemodea...${NC}"
cp -r "$SCRIPT_DIR/gamemodes/"* "$SERVER_DIR/gamemodes/" 2>/dev/null || true

# 5. Instaliraj zavisnosti + Xvfb za CEF
echo -e "${YELLOW}[5/6] Instalacija zavisnosti + Xvfb (POTREBNO ZA CEF)...${NC}"
if command -v apt-get &> /dev/null; then
    apt-get update -qq
    apt-get install -y -qq xvfb libglib2.0-0 libnss3 libatk1.0-0 libatk-bridge2.0-0 \
        libcups2 libdrm2 libxkbcommon0 libxcomposite1 libxdamage1 libxrandr2 \
        libgbm1 libpango-1.0-0 libcairo2 libasound2 libxshmfence1 screen 2>/dev/null || true
fi
# Pokreni Xvfb
killall Xvfb 2>/dev/null || true
Xvfb :99 -screen 0 1024x768x24 -ac &
sleep 1
grep -q 'DISPLAY=:99' /root/.bashrc 2>/dev/null || echo 'export DISPLAY=:99' >> /root/.bashrc

# 6. Prava
echo -e "${YELLOW}[6/6] Postavljanje prava...${NC}"
chmod +x "$SERVER_DIR/omp-server" 2>/dev/null || true
chmod -R 755 "$SERVER_DIR/plugins/" 2>/dev/null || true
chmod -R 755 "$SERVER_DIR/components/" 2>/dev/null || true
chmod -R 777 "$SERVER_DIR/scriptfiles/" 2>/dev/null || true

echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN}  DEPLOY ZAVRSEN!${NC}"
echo -e "${GREEN}============================================${NC}"
echo ""
echo "  Server: $SERVER_DIR"
echo "  IP: 217.156.22.164:7774"
echo ""
echo "  POKRETANJE:"
echo "    export DISPLAY=:99"
echo "    cd $SERVER_DIR"
echo "    ./omp-server"
echo ""
echo "  SA SCREEN:"
echo "    screen -AmdS unicate bash -c 'export DISPLAY=:99; ./omp-server'"
echo ""
echo "  FALI TI:"
echo "    1. fg-ogc.amx - kompajliraj na Windowsu!"
echo "    2. omp-server - skini sa https://github.com/openmultiplayer/open.mp/releases"
echo "    3. Otvori port 7774 na firewall-u"
