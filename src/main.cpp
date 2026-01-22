#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <future>

// LibDataChannel includes
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono_literals;

void log(const std::string& msg) {
    std::cout << "[PixelStream] " << msg << std::endl;
}

int main() {
    // 1. Setup Configuration (STUN Server)
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    // We need to keep the PeerConnection alive outside the callback scope
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;

    // 2. Connect to Signaling Server
    rtc::WebSocket ws;
    std::promise<void> exit_signal;

    ws.onOpen([&]()
    {
        log("WebSocket Connected to Signaling Server.");
    });

    ws.onMessage([&](auto data)
    {
        if (!std::holds_alternative<std::string>(data)) return;

        // Parse Signaling Message
        json msg = json::parse(std::get<std::string>(data));
        
        // A. Handle SDP (Offer/Answer)
        if (msg.contains("description"))
        {
            std::string type = msg["description"]["type"];
            std::string sdp = msg["description"]["sdp"];
            log("Received SDP: " + type);

            // Create the PeerConnection if it doesn't exist
            if (!pc) {
                pc = std::make_shared<rtc::PeerConnection>(config);

                // Setup ICE Candidate Handler
                pc->onLocalCandidate([&ws](rtc::Candidate candidate)
                    {
                        json msg;
                        msg["candidate"] = candidate.candidate();
                        msg["sdpMid"] = candidate.mid();
                        //msg["sdpMLineIndex"] = candidate.sdpMLineIndex;
                        ws.send(msg.dump());
                    });

                // Setup State Change Handler
                pc->onStateChange([](rtc::PeerConnection::State state)
                    {
                       log("State changed: " + std::to_string((int)state));
                    });

                // Setup Data Channel Handler (Wait for Browser to create it)
                pc->onDataChannel([&dc](std::shared_ptr<rtc::DataChannel> incoming_dc)
                    {
                        log("DataChannel Received: " + incoming_dc->label());
                        dc = incoming_dc;

                        dc->onOpen([&dc]()
                            {
                                log("DataChannel OPEN! P2P connection established.");
                                dc->send("Hello from C++ Host!");
                            });

                        dc->onMessage([](auto data)
                            {
                                if (std::holds_alternative<std::string>(data)) {
                                    log("Received from Browser: " + std::get<std::string>(data));
                                }
                            });
                    });
                
                pc->onLocalDescription([&ws](rtc::Description description)
                    {
                        json msg;
                        msg["description"]["type"] = description.typeString();
                        msg["description"]["sdp"] = std::string(description);
                        ws.send(msg.dump());
                        log("Sent Answer SDP to Browser.");
                    });
            }

            // Set Remote Description (The Browser's Offer)
            pc->setRemoteDescription(rtc::Description(sdp, type));

            // If it's an Offer, we must create an Answer
            if (type == "offer") {
                pc->setLocalDescription(); // Generate Answer automatically
            }
        }
        
        // B. Handle ICE Candidates
        else if (msg.contains("candidate")) {
             if (pc) {
                 log("Received Candidate");
                 pc->addRemoteCandidate(rtc::Candidate(
                     msg["candidate"], 
                     msg["sdpMid"]
                 ));
             }
        }
    });
    

    ws.onClosed([&]() { log("WebSocket Closed."); exit_signal.set_value(); });
    ws.onError([&](std::string e) { log("Error: " + e); exit_signal.set_value(); });

    ws.open("ws://localhost:8080");

    // launch
    log("Waiting for Browser to initiate connection...");
    auto future = exit_signal.get_future();
    future.wait();

    return 0;
}
