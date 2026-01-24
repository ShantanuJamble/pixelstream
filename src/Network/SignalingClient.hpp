#pragma once
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <string>

using json = nlohmann::json;

class SignalingClient
{
public:
    // Callbacks for events
    std::function<void()> onConnect;
    std::function<void(std::string sdp, std::string type)> onSdpReceived;
    std::function<void(std::string candidate, std::string mid)> onCandidateReceived;

    void connect(const std::string& url);
    void sendSdp(const std::string& sdp, const std::string& type);
    void sendCandidate(const std::string& candidate, const std::string& mid);
    void run(); // blocking run for main thread

private:
    rtc::WebSocket m_ws;
    std::promise<void> m_exit_signal;
};