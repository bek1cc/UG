#!/bin/bash
# ===== UG Auto-Start =====
# Pokrece AI backend + open.mp server
# Samo pokreni: ./start.sh

cd "$(dirname "$0")"

echo "============================================"
echo "  UG Server Auto-Start"
echo "============================================"

# ===== 1. AI BACKEND =====
# Check if backend is already running
if curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
  echo "[UG] ✓ AI Backend already running"
else
  echo "[UG] Starting AI Backend..."

  # Kill old backend
  pkill -f "node.*ai-backend/server.js" 2>/dev/null
  sleep 1

  # Install deps if needed
  if [ ! -d "ai-backend/node_modules" ]; then
    echo "[UG] Installing backend dependencies..."
    cd ai-backend && npm install --production 2>&1 && cd ..
  fi

  # Start backend
  cd ai-backend
  nohup node server.js >> /tmp/ug-ai-backend.log 2>&1 &
  BACKEND_PID=$!
  cd ..

  # Wait for backend
  for i in $(seq 1 15); do
    if curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
      echo "[UG] ✓ AI Backend ready! (PID: $BACKEND_PID)"
      break
    fi
    sleep 1
  done

  if ! curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
    echo "[UG] ✗ Backend failed! Check: cat /tmp/ug-ai-backend.log"
  fi
fi

# ===== 2. OPEN.MP SERVER =====
echo "[UG] Starting open.mp server..."

if [ -f "open.mp/omp-server" ]; then
  cd "open.mp" && ./omp-server
elif [ -f "# open.mp/omp-server" ]; then
  cd "# open.mp" && ./omp-server
else
  echo "[UG] ERROR: Cannot find omp-server!"
  exit 1
fi

# Cleanup backend on exit
echo "[UG] Shutting down..."
pkill -f "node.*ai-backend/server.js" 2>/dev/null
