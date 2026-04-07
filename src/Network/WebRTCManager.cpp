#include "WebRTCManager.hpp"
#include <iostream>

WebRTCManager::WebRTCManager()
{
    rtc::Configuration config;
    // No video track added — video goes over a DataChannel instead.
    // This means the SDP only negotiates DataChannels, which works reliably.
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

void WebRTCManager::createVideoChannel()
{
    // Create a second DataChannel for video frames AFTER the connection is up.
    // This uses SCTP in-band signaling (DCEP) — no SDP renegotiation needed.
    m_videoDc = m_pc->createDataChannel("video");
    m_videoDc->onOpen([]() {
        std::cout << "[WebRTC] Video DataChannel OPENED." << std::endl;
    });
    m_videoDc->onClosed([]() {
        std::cout << "[WebRTC] Video DataChannel CLOSED." << std::endl;
    });
    std::cout << "[WebRTC] Video DataChannel created." << std::endl;
}

void WebRTCManager::sendVideoData(const std::vector<uint8_t>& data)
{
    if (m_videoDc && m_videoDc->isOpen()) {
        m_videoDc->send(reinterpret_cast<const std::byte*>(data.data()), data.size());
    }
}