#include "VideoPipeline.hpp"
#include <iostream>
#include <gst/app/gstappsink.h>

VideoPipeline& VideoPipeline::getInstance()
{
    static VideoPipeline instance;
    return instance;
}

VideoPipeline::VideoPipeline()
{
    m_initialized = initGStreamer();
    if (m_initialized)
    {
        std::cout << "[VideoPipeline] Singleton Initialized Successfully." << std::endl;
    }
    else
    {
        std::cerr << "[VideoPipeline] Singleton Failed to Initialize." << std::endl;
    }
}

VideoPipeline::~VideoPipeline()
{
    stop();
    if (m_appSink) {
        gst_object_unref(m_appSink);
        m_appSink = nullptr;
    }
    if (m_pipeline) {
        gst_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

bool VideoPipeline::initGStreamer()
{
    GError* error = nullptr;

    // Init GStreamer
    if (!gst_init_check(nullptr, nullptr, &error))
    {
        std::cerr << "GStreamer Init Error: " << (error ? error->message : "Unknown") << std::endl;
        return false;
    }

    // Build Pipeline
    // PIPELINE: Capture -> Encode -> Packetize (RTP) -> AppSink (C++ Callback)
    // 1. tune=zerolatency, can play around based on requirements..
    // 2. speed-preset=ultrafast: Sacrifice quality for speed.
    // 3. rtph264pay: Wraps H.264 into RTP packets (seting up data WebRTC).
    // 4. appsink: Hand the data to us.
    const std::string pipeline_str = 
        "d3d11screencapturesrc ! videoconvert ! "
        "x264enc tune=zerolatency speed-preset=ultrafast key-int-max=60 ! "
        "rtph264pay config-interval=1 ! "
        "appsink name=pixelstreamvideosink emit-signals=true sync=false";

    m_pipeline = gst_parse_launch(pipeline_str.c_str(), &error);

    if (error || !m_pipeline)
    {
        std::cerr << "Pipeline Parse Error: " << (error ? error->message : "Unknown") << std::endl;
        return false;
    }

    // Find AppSink
    m_appSink = gst_bin_get_by_name(GST_BIN(m_pipeline), "pixelstreamvideosink");
    if (!m_appSink) return false;

    return true;
}

bool VideoPipeline::start(VideoCallback callback)
{
    if (!isReady())
    {
        std::cerr << "[Error] Cannot start: Pipeline not initialized." << std::endl;
        return false;
    }
    if (m_isRunning)
    {
        std::cerr << "[Error] Cannot start: Pipeline already running." << std::endl;
        return false;
    }
    
    m_videoCallback = callback;
    
    g_signal_connect(m_appSink, "new-sample", G_CALLBACK(onNewSampleReceived), this);
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    m_isRunning = true;
    return true;
}

void VideoPipeline::stop() {
    // Only resets state and doesn't cleanup fully so we can start pipeline again.
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
    }
    m_isRunning = false;
}

GstFlowReturn VideoPipeline::onNewSampleReceived(GstElement* sink, const gpointer data) {
    const auto* self = static_cast<VideoPipeline*>(data);
    GstSample* sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample) {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        if (buffer) {
            gsize size = gst_buffer_get_size(buffer);
            std::vector<uint8_t> packet_data(size);
            gst_buffer_extract(buffer, 0, packet_data.data(), size);
            
            if (self->m_videoCallback) self->m_videoCallback(packet_data);
        }
        gst_sample_unref(sample);
    }
    return GST_FLOW_OK;
}