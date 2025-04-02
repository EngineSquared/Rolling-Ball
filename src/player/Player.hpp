#pragma once

namespace Game {
    struct Player {
        // In m/s
        glm::vec3 maxSpeed{4.0f, 10.0f, 4.0f};
        // In m/s^2
        glm::vec3 acceleration{5000.0f, 20000.0f, 5000.0f};
    };
}