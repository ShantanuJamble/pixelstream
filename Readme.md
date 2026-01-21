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
To prove the C++ app is actually talking to the outside world, open a new PowerShell window and use wscat (requires Node.js) or a python script.

**Option A** (Node.js/wscat):
```
npx wscat -c ws://localhost:8080
```

**Option B (Browser):** Go to WebSocket King and connect to ws://localhost:8080.

Verification:

Wait a few seconds.

You should see JSON messages appearing in your 3rd party client: {"type":"ping", "payload":"Hello from C++"}.
Type a message in the 3rd party client and hit Enter.
Look at your C++ Console; it should print: [C++] Received: [Your Message].