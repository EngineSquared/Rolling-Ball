#pragma once

#include <chrono>

#include <glm/glm.hpp>

namespace Game {
    struct Player {
        // In m/s
        glm::vec3 maxSpeed{15.0f, 0.0f, 15.0f};
        // In m/s^2
        glm::vec3 acceleration{5000.0f, 0.0f, 5000.0f};
        // Last jump
        std::chrono::time_point<std::chrono::steady_clock> lastJump;
        // Jump impulse
        float jumpImpulse{3800.0f};
        // Terrain contacts
        int terrainContacts{0};
        // Finish contacts
        int finishContacts{0};
    };
}