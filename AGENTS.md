# AGENTS.md

## Project Snapshot
- `PixelStream` is a C++17 WebRTC sender + physics simulator, with a Python WebSocket signaling relay and browser client (`web/index.html`).
- Runtime wiring happens in `src/main.cpp` via callback composition (Signaling <-> WebRTC, DataChannel -> Physics).

## Architecture You Need First
- `src/Network/SignalingClient.*`: wraps `rtc::WebSocket`; exchanges JSON messages with keys `description` or `candidate`.
- `src/Network/WebRTCManager.*`: owns `rtc::PeerConnection`, emits local SDP/ICE, accepts remote SDP/ICE, creates a send-only H.264 video track.
- `src/SimEngine/PhysicsEngine.*`: authoritative simulation loop on a worker thread; sends 8-byte binary payloads (`float x`, `float y`, little-endian).
- `src/Media/VideoPipeline.*`: singleton GStreamer pipeline (`d3d11screencapturesrc ! x264enc ! rtph264pay ! appsink`) producing RTP packet bytes.
- `signaling/server.py`: simple broadcast relay to all other connected clients; no rooms/auth/state.

## Critical Data Flows
- Signaling schema must stay compatible across C++ and browser:
  - SDP: `{ "description": { "sdp": "...", "type": "offer|answer" } }`
  - ICE: `{ "candidate": "...", "sdpMid": "..." }`
- Browser physics render path (`web/index.html`) assumes exactly 8 bytes on DataChannel and decodes with `DataView.getFloat32(..., true)`.
- `WebRTCManager::sendVideo()` strips 12-byte RTP header before `Track::send()` to avoid double RTP headers.
- `main.cpp` currently starts `VideoPipeline` but only logs packet size; it does **not** forward video packets to `WebRTCManager::sendVideo()` yet.

## Build / Run Workflow (Observed)
- Bootstrap/generate solution (Windows): run `build_project.bat` (uses vcpkg toolchain and VS 2022 generator).
- Build target from generated `build/PixelStream.sln` (IDE or CMake build command).
- Start signaling server: `python signaling/server.py`.
- Start native app: `build/Debug/PixelStream.exe`.
- Open `web/index.html` in browser and click **Connect & Play**.

## Environment / Dependencies
- CMake requires `LibDataChannel` and `nlohmann_json` via `vcpkg.json`.
- Windows build requires `GSTREAMER_1_0_ROOT_MSVC_X86_64` env var; configure fails if missing (`CMakeLists.txt`).
- Python signaling server depends on `websockets`.

## Codebase-Specific Editing Guidance
- Prefer preserving callback-based orchestration in `main.cpp` instead of introducing global state.
- Keep physics thread lifecycle safe: `PhysicsEngine::run()` calls `stop()` first, and destructor joins thread.
- If changing DataChannel payload format, update **both** `PhysicsEngine::serialize()` and browser decode logic in `web/index.html` together.
- If changing signaling JSON fields, update all three sides: `SignalingClient.cpp`, `signaling/server.py` (relay assumptions), and `web/index.html`.
- If changing video pipeline/codec settings, verify compatibility with `WebRTCManager::setupVideoTrack()` H.264 profile and payload type.

