## About
This is my exploratory project to steup webRTC based stream.
- Currently supports data channel.
- Video track is currently in dev. 
    - Currently works in a very hackish way. I am funneling lot of Jpegs though data channel instead of video track. (Not scalable)
    - Try and use gstreamer's tried and tested solution [rswebrtc](https://gstreamer.freedesktop.org/documentation/rswebrtc/index.html?gi-language=c) to get video track working.    

## Building/Generating:
You can use build_project.bat to generate and build the project. 

## Testing WebRTC setup
### Step 1: Run the Signaling Server
```
python signaling/server.py
```
Expected Output: Signaling Server running on ws://localhost:8080

### Step 2: Start the C++ Client
Start the executable under build folder in a terminal or powershell.
```
[C++] Connecting to ws://localhost:8080...
[C++] WebSocket Connected to Signaling Server!
[C++] Sending Ping...
```

### Step 3: Verify with a 3rd Party Client
Open web/index.html in your browser and click "Connect & Play".

- Expected Result: A ball bouncing smoothly at 60 FPS.


## Walkthrough

|                                             **GStreamer Demo**                                             |                                          **Physics Sim Demo**                                           |
|:----------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------:|
| [![Video Demo with Gstreamer](https://img.youtube.com/vi/ziR86J7MJTQ/0.jpg)](https://youtu.be/ziR86J7MJTQ) | [![Video Demo Physics sim](https://img.youtube.com/vi/hQkQeegANvk/0.jpg)](https://youtu.be/hQkQeegANvk) |
