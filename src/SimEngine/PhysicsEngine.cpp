#include "PhysicsEngine.hpp"
#include <chrono>
#include <cstring>
#include <iostream>

PhysicsEngine::~PhysicsEngine()
{
    stop();
}

void PhysicsEngine::run(SendFunction sendCallback)
{
    stop();
    isRunning = true;
    
    gameThread = std::thread(&PhysicsEngine::loop, this, sendCallback);
    
    //gameThread.detach(); 
}

void PhysicsEngine::stop()
{
    if (isRunning) {
        std::cout << "[Physics] Stopping Engine..." << std::endl;
        isRunning = false;
    }
    
    // This prevents "Zombie Threads" from running in the background.
    if (gameThread.joinable()) {
        gameThread.join();
        std::cout << "[Physics] Engine Stopped Cleanly." << std::endl;
    }
}

std::vector<std::uint8_t> PhysicsEngine::serialize(const Ball& b)
{
    std::vector<std::uint8_t> data(8);
    std::memcpy(data.data(), &b.x, sizeof(float));
    std::memcpy(data.data() + 4, &b.y, sizeof(float));
    return data;
}

void PhysicsEngine::loop(SendFunction sendCallback)
{
    Ball ball;
    const float width = 800.0f;
    const float height = 600.0f;
    auto lastTime = std::chrono::high_resolution_clock::now();

    std::cout << "[Physics] Engine Started." << std::endl;

    while (isRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Physics Logic
        ball.x += ball.vx * deltaTime;
        ball.y += ball.vy * deltaTime;

        if (ball.x < ball.radius) { ball.x = ball.radius; ball.vx *= -1; }
        if (ball.x > width - ball.radius) { ball.x = width - ball.radius; ball.vx *= -1; }
        if (ball.y < ball.radius) { ball.y = ball.radius; ball.vy *= -1; }
        if (ball.y > height - ball.radius) { ball.y = height - ball.radius; ball.vy *= -1; }

        // Send Data
        sendCallback(serialize(ball));
        // try {
        //     sendCallback(serialize(ball));
        // }
        // catch (const std::exception& e) {
        //     std::cerr << "[Physics] Error sending data: " << e.what() << std::endl;
        //     // isRunning = false; 
        // }
        // catch (...) {
        //     std::cerr << "[Physics] Unknown error sending data." << std::endl;
        // }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}