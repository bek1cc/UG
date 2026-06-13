#!/bin/bash
# UG Server + AI Panel - Linux startup
echo "============================================"
echo "   UG Server - Auto Start"
echo "   Server + AI Dev Panel"
echo "============================================"
echo ""

# Provjeri Node.js
if ! command -v node &> /dev/null; then
    echo "[GRESKA] Node.js nije instaliran! Pokrecem samo server..."
    echo ""
    cd open.mp && exec ./omp-server
fi

# Pokreni AI backend u pozadini
echo "[1/2] Pokrecem AI Dev Panel backend..."
cd "$(dirname "$0")/ai-backend"
node server.js &
AI_PID=$!
cd "$(dirname "$0")"

sleep 2
echo "[OK] AI Dev Panel backend pokrenut (PID: $AI_PID) na http://127.0.0.1:3777"
echo ""

# Pokreni open.mp server
echo "[2/2] Pokrecem open.mp server..."
cd open.mp
./omp-server

# Kada server zavrsi, ugasi backend
echo ""
echo "[INFO] Server zatvoren. Gasim AI backend..."
kill $AI_PID 2>/dev/null
