#pragma once
#include <vector>
#include <atomic>
#include <functional>
#include <thread>

struct Ball
{
    float x = 400.0f; float y = 300.0f;
    float vx = 300.0f; float vy = 200.0f;
    float radius = 20.0f;
};

class PhysicsEngine
{
public:
    PhysicsEngine() = default;
    ~PhysicsEngine();
    // Defines a generic way to send data out
    using SendFunction = std::function<void(const std::vector<std::uint8_t>&)>;

    void run(SendFunction sendCallback);
    void stop();

private:
    std::atomic<bool> isRunning{false};
    std::thread gameThread;
    
    std::vector<std::uint8_t> serialize(const Ball& b);
    void loop(SendFunction sendCallback);
};