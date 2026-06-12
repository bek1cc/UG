#!/bin/bash
# ===== UG AI Backend - Auto-Start Watchdog =====
# Ova skripta osigurava da je backend UVIJEK pokrenut
# Pokrece se jednom i ostaje u pozadini

DIR="/home/z/my-project/UG/ai-backend"
LOG="/tmp/ug-ai-backend.log"

while true; do
  # Check if backend is alive
  if ! curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
    echo "[$(date)] Starting AI Backend..." >> "$LOG"
    cd "$DIR"
    node server.js >> "$LOG" 2>&1 &
    BACKEND_PID=$!
    echo "[$(date)] Backend started (PID: $BACKEND_PID)" >> "$LOG"
    # Wait a bit
    sleep 5
    if ! curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
      echo "[$(date)] Backend failed to start, retrying in 10s..." >> "$LOG"
      sleep 10
    fi
  fi
  # Check every 30 seconds
  sleep 30
done
