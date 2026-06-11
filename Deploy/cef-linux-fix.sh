#!/bin/bash
# ============================================================
#  CEF FIX ZA LINUX HOST
#  Pokreni ovo na serveru kao root
# ============================================================

echo "[1] Instalacija Xvfb (virtualni X server - CEF ga MORA imati)"
if command -v apt-get &> /dev/null; then
    apt-get update -qq
    apt-get install -y -qq xvfb libglib2.0-0 libnss3 libatk1.0-0 libatk-bridge2.0-0 libcups2 libdrm2 libxkbcommon0 libxcomposite1 libxdamage1 libxrandr2 libgbm1 libpango-1.0-0 libcairo2 libasound2 libxshmfence1 2>/dev/null
elif command -v yum &> /dev/null; then
    yum install -y xorg-x11-server-Xvfb glib2 nss atk at-spi2-atk cups-libs libdrm libxkbcommon libXcomposite libXdamage libXrandr mesa-libgbm pango cairo alsa-lib 2>/dev/null
fi

echo "[2] Pokretanje Xvfb na display :99"
# Ubij stari Xvfb ako postoji
killall Xvfb 2>/dev/null || true
# Pokreni novi
Xvfb :99 -screen 0 1024x768x24 -ac &
sleep 1

echo "[3] Postavi DISPLAY environment varijablu"
export DISPLAY=:99
echo 'export DISPLAY=:99' >> /root/.bashrc
echo 'export DISPLAY=:99' >> /home/samp/.bashrc 2>/dev/null || true

echo "[4] Provjera da li Xvfb radi"
if xdpyinfo -display :99 >/dev/null 2>&1; then
    echo "Xvfb RADI na :99!"
else
    echo "GRESKA: Xvfb ne radi! Pokusaj rucno: Xvfb :99 -screen 0 1024x768x24 &"
fi

echo "[5] Pokretanje open.mp servera sa DISPLAY"
echo ""
echo "SADA POKRENI SERVER OVAKO:"
echo "  export DISPLAY=:99"
echo "  cd /home/samp/server"
echo "  ./omp-server"
echo ""
echo "ILIO SA SCREEN:"
echo "  screen -AmdS unicate bash -c 'export DISPLAY=:99; ./omp-server'"
echo ""
echo "PROVJERA LOGOVA:"
echo "  tail -f /home/samp/server/log.txt | grep -i cef"
