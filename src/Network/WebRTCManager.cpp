#include "WebRTCManager.hpp"
#include <iostream>

WebRTCManager::WebRTCManager()
{
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    m_pc = std::make_shared<rtc::PeerConnection>(config);
    
    m_pc->onStateChange([](rtc::PeerConnection::State state) {
        std::cout << "[WebRTC] State: " << state << std::endl;
    });
}

void WebRTCManager::handleRemoteSdp(const std::string& sdp, const std::string& type)
{
    m_pc->setRemoteDescription(rtc::Description(sdp, type));
    if (type == "offer") {
        m_pc->setLocalDescription();
    }
}

void WebRTCManager::handleRemoteCandidate(const std::string& candidate, const std::string& mid)
{
    m_pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}

void WebRTCManager::onDataChannel(DataChannelCallback callback)
{
    m_pc->onDataChannel([callback](std::shared_ptr<rtc::DataChannel> incoming_dc) {
        callback(incoming_dc);
    });
}

void WebRTCManager::onLocalDescription(LocalDescriptionCallback callback)
{
    m_pc->onLocalDescription([callback](rtc::Description desc) {
        callback(std::string(desc), desc.typeString());
    });
}

void WebRTCManager::onLocalCandidate(LocalCandidateCallback callback)
{
    m_pc->onLocalCandidate([callback](rtc::Candidate candidate) {
        callback(candidate.candidate(), candidate.mid());
    });
}