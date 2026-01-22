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

- Expected Result: A blue ball bouncing smoothly at 60 FPS.
- Stress Test: Refresh the page and reconnect multiple times. The C++ app should handle the thread restart 
and only one clinet (latest) should remain active.



## Walkthrough
[![Video demo](https://img.youtube.com/vi/hQkQeegANvk/maxresdefault.jpg)](https://youtu.be/hQkQeegANvk)