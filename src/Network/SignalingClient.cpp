#include "SignalingClient.hpp"
#include <iostream>

void SignalingClient::connect(const std::string& url)
{
    m_ws.onOpen([this]()
        {
            if (onConnect) onConnect();
        });

    m_ws.onMessage([this](auto data)
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

    m_ws.onClosed([this]() { m_exit_signal.set_value(); });
    m_ws.open(url);
}

void SignalingClient::sendSdp(const std::string& sdp, const std::string& type)
{
    json msg;
    msg["description"]["sdp"] = sdp;
    msg["description"]["type"] = type;
    m_ws.send(msg.dump());
}

void SignalingClient::sendCandidate(const std::string& candidate, const std::string& mid)
{
    json msg;
    msg["candidate"] = candidate;
    msg["sdpMid"] = mid;
    m_ws.send(msg.dump());
}

void SignalingClient::run()
{
    auto future = m_exit_signal.get_future();
    future.wait();
}