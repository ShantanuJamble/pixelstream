#pragma once
#include <rtc/rtc.hpp>
#include <memory>
#include <functional>

class WebRTCManager
{
public:
    using DataChannelCallback = std::function<void(std::shared_ptr<rtc::DataChannel>)>;
    using LocalDescriptionCallback = std::function<void(std::string sdp, std::string type)>;
    using LocalCandidateCallback = std::function<void(std::string candidate, std::string mid)>;

    WebRTCManager();
    
    // Connection Logic
    void handleRemoteSdp(const std::string& sdp, const std::string& type);
    void handleRemoteCandidate(const std::string& candidate, const std::string& mid);

    // Hooks
    void onDataChannel(DataChannelCallback callback);
    void onLocalDescription(LocalDescriptionCallback callback);
    void onLocalCandidate(LocalCandidateCallback callback);

    // Video Logic
    void setupVideoTrack();
    void sendVideo(const std::vector<uint8_t>& data);

private:
    std::shared_ptr<rtc::PeerConnection> m_pc;
    std::shared_ptr<rtc::DataChannel> m_dc;
    std::shared_ptr<rtc::Track> m_videoTrack;
};