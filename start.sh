#!/bin/bash
# ===== UG Server + AI Backend Auto-Start =====
# Pokrece AI backend pa onda open.mp server
# Sve radi automatski - nema rucnog pokretanja

cd "$(dirname "$0")"

echo "[UG] ============================================"
echo "[UG] UG Server Auto-Start"
echo "[UG] ============================================"

# ===== AI BACKEND =====
echo "[UG] Starting AI Backend..."

# Kill old backend if running
pkill -f "node.*ai-backend/server.js" 2>/dev/null
sleep 1

# Install deps if needed
if [ ! -d "ai-backend/node_modules" ]; then
  echo "[UG] Installing backend dependencies..."
  cd ai-backend && npm install --production 2>&1 && cd ..
fi

# Start backend in background
cd ai-backend
node server.js >> /tmp/ug-ai-backend.log 2>&1 &
BACKEND_PID=$!
cd ..

# Wait for backend to be ready (up to 10s)
echo "[UG] Waiting for backend..."
for i in $(seq 1 10); do
  if curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
    echo "[UG] ✓ AI Backend ready! (PID: $BACKEND_PID)"
    break
  fi
  sleep 1
done

if ! curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
  echo "[UG] ✗ WARNING: AI Backend failed to start!"
  echo "[UG]   Check log: cat /tmp/ug-ai-backend.log"
  echo "[UG]   Continuing without AI backend..."
fi

# ===== OPEN.MP SERVER =====
echo "[UG] Starting open.mp server..."
if [ -f "open.mp/omp-server" ]; then
  cd "open.mp" && ./omp-server
elif [ -f "# open.mp/omp-server" ]; then
  cd "# open.mp" && ./omp-server
else
  echo "[UG] ERROR: Cannot find omp-server!"
  echo "[UG] Available directories:"
  ls -d */ 2>/dev/null
  exit 1
fi

# ===== CLEANUP =====
echo "[UG] Shutting down AI Backend..."
kill $BACKEND_PID 2>/dev/null
wait $BACKEND_PID 2>/dev/null
echo "[UG] Goodbye!"
