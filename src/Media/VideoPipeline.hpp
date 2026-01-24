#pragma once
#include <vector>
#include <functional>
#include <gst/gst.h>

class VideoPipeline {
public:
    using VideoCallback = std::function<void(const std::vector<uint8_t>&)>;
    
    static VideoPipeline& getInstance();
    
    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator=(const VideoPipeline&) = delete;
    
    bool start(VideoCallback callback); 
    void stop();
    bool isReady() const { return m_initialized; }

private:
    VideoPipeline();
    ~VideoPipeline();

    bool initGStreamer();
    static GstFlowReturn onNewSampleReceived(GstElement* sink, const gpointer data);

    GstElement* m_pipeline = nullptr;
    GstElement* m_appSink = nullptr;
    VideoCallback m_videoCallback;
    bool m_initialized = false;
    bool m_isRunning = false;
};