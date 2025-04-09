#pragma once

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Game {
    enum class TerrainType {
        Flat,
        Ramp,
        CurveLeft,
        CurveRight,
        Wave,
        Gap
    };

    struct TerrainPiece {
        TerrainType type;

        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotationAngle;
    };

    struct Terrain {
        std::vector<TerrainPiece> pieces;

        int segmentCount = 10;
        glm::vec3 segmentsPositionOffset{0.0f, 0.0f, 0.0f};
        glm::vec3 segmentsGapPositionOffset{0.0f, 0.0f, 40.0f};
        glm::vec3 segmentsScale{4.0f, 4.0f, 4.0f};
        glm::quat segmentsRotation{0.0f, 0.7071f, 0.0f, 0.7071f};

        // Random seed
        std::mt19937 rng;
        Terrain() {
            std::random_device rd;
            rng.seed(rd());
        }
    };
}