#include "SignalingClient.hpp"
#include <iostream>

void SignalingClient::connect(const std::string& url)
{
    ws.onOpen([this]()
        {
            if (onConnect) onConnect();
        });

    ws.onMessage([this](auto data)
        {
            if (!std::holds_alternative<std::string>(data)) return;
            
            try
            {
                json msg = json::parse(std::get<std::string>(data));
                
                if (msg.contains("description")) {
                    if (onSdpReceived) {
                        onSdpReceived(msg["description"]["sdp"], msg["description"]["type"]);
                    }
                } else if (msg.contains("candidate")) {
                    if (onCandidateReceived) {
                        onCandidateReceived(msg["candidate"], msg["sdpMid"]);
                    }
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "[Signaling] JSON Error: " << e.what() << std::endl;
            }
        });

    ws.onClosed([this]() { exitSignal.set_value(); });
    ws.open(url);
}

void SignalingClient::sendSdp(const std::string& sdp, const std::string& type)
{
    json msg;
    msg["description"]["sdp"] = sdp;
    msg["description"]["type"] = type;
    ws.send(msg.dump());
}

void SignalingClient::sendCandidate(const std::string& candidate, const std::string& mid)
{
    json msg;
    msg["candidate"] = candidate;
    msg["sdpMid"] = mid;
    ws.send(msg.dump());
}

void SignalingClient::run()
{
    auto future = exitSignal.get_future();
    future.wait();
}