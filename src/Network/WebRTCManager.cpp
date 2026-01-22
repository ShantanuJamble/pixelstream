#include "WebRTCManager.hpp"
#include <iostream>

WebRTCManager::WebRTCManager()
{
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    pc = std::make_shared<rtc::PeerConnection>(config);
    
    pc->onStateChange([](rtc::PeerConnection::State state) {
        std::cout << "[WebRTC] State: " << state << std::endl;
    });
}

void WebRTCManager::handleRemoteSdp(const std::string& sdp, const std::string& type)
{
    pc->setRemoteDescription(rtc::Description(sdp, type));
    if (type == "offer") {
        pc->setLocalDescription();
    }
}

void WebRTCManager::handleRemoteCandidate(const std::string& candidate, const std::string& mid)
{
    pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}

void WebRTCManager::onDataChannel(DataChannelCallback callback)
{
    pc->onDataChannel([callback](std::shared_ptr<rtc::DataChannel> incoming_dc) {
        callback(incoming_dc);
    });
}

void WebRTCManager::onLocalDescription(LocalDescriptionCallback callback)
{
    pc->onLocalDescription([callback](rtc::Description desc) {
        callback(std::string(desc), desc.typeString());
    });
}

void WebRTCManager::onLocalCandidate(LocalCandidateCallback callback)
{
    pc->onLocalCandidate([callback](rtc::Candidate candidate) {
        callback(candidate.candidate(), candidate.mid());
    });
}