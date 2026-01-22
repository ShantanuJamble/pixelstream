#include <iostream>
#include "Network/SignalingClient.hpp"
#include "Network/WebRTCManager.hpp"
#include "SimEngine/PhysicsEngine.hpp"

int main() {
    SignalingClient signaling;
    WebRTCManager webrtc;
    PhysicsEngine engine;

    // 1. Wire WebRTC -> Signaling (Outbound)
    webrtc.onLocalDescription([&](std::string sdp, std::string type) {
        signaling.sendSdp(sdp, type);
    });

    webrtc.onLocalCandidate([&](std::string candidate, std::string mid) {
        signaling.sendCandidate(candidate, mid);
    });

    // 2. Wire Signaling -> WebRTC (Inbound)
    signaling.onSdpReceived = [&](std::string sdp, std::string type) {
        webrtc.handleRemoteSdp(sdp, type);
    };

    signaling.onCandidateReceived = [&](std::string candidate, std::string mid) {
        webrtc.handleRemoteCandidate(candidate, mid);
    };

    // 3. Wire WebRTC -> Physics (Start Engine when channel opens)
    webrtc.onDataChannel([&](std::shared_ptr<rtc::DataChannel> dc) {
        std::cout << "[Main] DataChannel Ready. Wiring Physics..." << std::endl;
        
        dc->onOpen([dc, &engine]() {
            // Start the engine, passing a lambda to send data
            engine.run([dc](const std::vector<std::uint8_t>& data) {
                 if (dc->isOpen()) {
                     dc->send(reinterpret_cast<const std::byte*>(data.data()), data.size());
                 }
            });
        });
    });

    // 4. Start
    std::cout << "[Main] Starting PixelStream..." << std::endl;
    signaling.connect("ws://localhost:8080");
    signaling.run();

    return 0;
}