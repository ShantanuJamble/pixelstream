#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <future>

// LibDataChannel includes
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono_literals;

int main() {
    // 1. Create a WebSocket client
    rtc::WebSocket ws;

    // 2. Promise to keep the main thread alive until we are done
    std::promise<void> exit_signal;

    // 3. Setup Callbacks
    ws.onOpen([]() {
        std::cout << "[C++] WebSocket Connected to Signaling Server!" << std::endl;
    });

    ws.onMessage([](auto data) {
        // Handle string messages
        if (std::holds_alternative<std::string>(data)) {
            std::cout << "[C++] Received: " << std::get<std::string>(data) << std::endl;
        }
    });

    ws.onClosed([&]() {
        std::cout << "[C++] WebSocket Closed." << std::endl;
        exit_signal.set_value();
    });

    ws.onError([&](std::string error) {
        std::cerr << "[C++] Error: " << error << std::endl;
        exit_signal.set_value();
    });

    // 4. Connect to Python Server
    std::string url = "ws://localhost:8080";
    std::cout << "[C++] Connecting to " << url << "..." << std::endl;
    ws.open(url);

    // 5. Send a test loop
    // In a real app, this is where we would send SDP offers
    std::thread sender([&]() {
        while (ws.isOpen()) {
            std::this_thread::sleep_for(2s);
            
            // Create a JSON message
            json msg;
            msg["type"] = "ping";
            msg["payload"] = "Hello from C++";
            
            std::cout << "[C++] Sending Ping..." << std::endl;
            ws.send(msg.dump());
        }
    });

    // Wait until connection closes
    auto future = exit_signal.get_future();
    future.wait();
    
    if (sender.joinable()) sender.join();
    return 0;
}