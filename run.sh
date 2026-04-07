#!/bin/bash
set -e

# Start the signaling server in the background
echo "[INFO] Starting signaling server..."
python3 signaling/server.py &
SERVER_PID=$!

# Function to clean up background processes on exit
cleanup() {
    echo "[INFO] Stopping signaling server..."
    kill $SERVER_PID
}
trap cleanup EXIT

# Wait a moment for the server to start
sleep 1

# Run the main application
echo "[INFO] Starting PixelStream application..."
if [ -f "build/PixelStream" ]; then
    ./build/PixelStream
else
    echo "[ERROR] PixelStream executable not found. Please build the project first."
    exit 1
fi

