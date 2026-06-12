#!/bin/bash
# ===== UG Server + AI Backend Auto-Start =====
# Pokrece AI backend pa onda open.mp server

cd "$(dirname "$0")"

echo "[UG] Starting AI Backend..."
# Kill old backend if running
pkill -f "node.*ai-backend/server.js" 2>/dev/null
sleep 1

# Start backend
cd ai-backend
node server.js > /tmp/ug-ai-backend.log 2>&1 &
BACKEND_PID=$!
cd ..

# Wait for backend to be ready
echo "[UG] Waiting for backend..."
for i in $(seq 1 10); do
  if curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
    echo "[UG] AI Backend ready! (PID: $BACKEND_PID)"
    break
  fi
  sleep 1
done

if ! curl -s http://localhost:3777/api/health > /dev/null 2>&1; then
  echo "[UG] WARNING: AI Backend failed to start! Check /tmp/ug-ai-backend.log"
  echo "[UG] Continuing without AI backend..."
fi

# Start open.mp server
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
echo "[UG] Shutting down AI Backend..."
kill $BACKEND_PID 2>/dev/null
