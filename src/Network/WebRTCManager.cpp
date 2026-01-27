#include "WebRTCManager.hpp"
#include <iostream>

WebRTCManager::WebRTCManager()
{
    rtc::Configuration config;
    //config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    m_pc = std::make_shared<rtc::PeerConnection>(config);
    
    m_pc->onStateChange([](rtc::PeerConnection::State state) {
        std::cout << "[WebRTC] State: " << state << std::endl;
    });
    setupVideoTrack();
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

void WebRTCManager::setupVideoTrack()
{
    if (m_videoTrack) return;

    // Create a Video Track (label: "video")
    m_videoTrack = m_pc->addTrack(rtc::Description::Video("video"));

    // Force H.264 Codec (Payload Type 96)
    // "42e01f" = Constrained Baseline Profile (Browser compatible)
    rtc::Description::Video media("video", rtc::Description::Direction::SendOnly);
    media.addH264Codec(96, "packetization-mode=1;profile-level-id=42e01f");
    m_videoTrack->setDescription(media);

    std::cout << "[WebRTC] Video Track Created." << std::endl;
}

void WebRTCManager::sendVideo(const std::vector<uint8_t>& data)
{
    if (!m_videoTrack || m_pc->state() != rtc::PeerConnection::State::Connected) return;

    // 2. STRIP RTP HEADER (The Fix)
    // GStreamer sends 12 byte RTP header. libdatachannel adds its own.
    // We must remove GStreamer's header to avoid "Double Header" corruption.
    if (data.size() > 12) {
        auto payload_ptr = reinterpret_cast<const std::byte*>(data.data() + 12);
        size_t payload_size = data.size() - 12;
        m_videoTrack->send(payload_ptr, payload_size);
    }
}