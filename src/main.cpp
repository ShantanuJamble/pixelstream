#include <iostream>
#include "Network/SignalingClient.hpp"
#include "Network/WebRTCManager.hpp"
#include "SimEngine/PhysicsEngine.hpp"
#include "Media/VideoPipeline.hpp"

int main() {

    VideoPipeline& video = VideoPipeline::getInstance();
    
    if (!video.isReady()) {
        std::cerr << "Fatal: Video System failed to load." << std::endl;
        return -1;
    }

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
    
    // 3. Wire DataChannel -> Physics + Video
    webrtc.onDataChannel([&](std::shared_ptr<rtc::DataChannel> dc) {
        std::cout << "[Main] DataChannel Ready. Wiring Physics + Video..." << std::endl;
        
        dc->onOpen([dc, &engine, &video, &webrtc]() {
            // Start the physics engine
            engine.run([dc](const std::vector<std::uint8_t>& data) {
                 if (dc->isOpen()) {
                     dc->send(reinterpret_cast<const std::byte*>(data.data()), data.size());
                 }
            });

            // Create a second DataChannel for video (SCTP in-band, no SDP renegotiation)
            webrtc.createVideoChannel();

            // Start the GStreamer pipeline — JPEG frames go over the video DataChannel
            video.start([&webrtc](const std::vector<uint8_t>& data) {
                webrtc.sendVideoData(data);
            });
            std::cout << "[Main] VideoPipeline started — streaming JPEG over DataChannel." << std::endl;
        });
    });
    
    // 4. Start
    std::cout << "[Main] Starting PixelStream..." << std::endl;
    signaling.connect("ws://localhost:8080");
    signaling.run();

    return 0;
}