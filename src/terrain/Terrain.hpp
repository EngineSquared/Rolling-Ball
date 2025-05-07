#pragma once

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Game {
    enum class TerrainType : int {
        Flat,
        Ramp,
        CurveLeft,
        CurveRight,
        Wave,
        Jump,
        JumpSingle,
        ObstacleA,
        ObstacleB,
        ObstacleC,
        MovingObstacleA,
        MovingObstacleB,
        Finish
    };

    struct TerrainPiece {
        TerrainType type;

        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotationAngle;
    };

    struct Terrain {
        std::vector<TerrainPiece> pieces;

        int segmentCount = 30;
        glm::vec3 segmentsPositionOffset{0.0f, 0.0f, 0.0f};
        glm::vec3 segmentsGapPositionOffset{0.0f, -1.26f, 20.0f};
        glm::vec3 segmentsScale{2.0f, 2.0f, 2.0f};
        glm::quat segmentsRotation{0.7067f, 0.0222f, 0.7067f, 0.0222f};

        // Random seed
        std::mt19937 rng;
        Terrain() {
            std::random_device rd;
            rng.seed(rd());
        }
    };

    struct SegmentMetrics {
        float lengthZ;
        float heightDeltaY;
    };
}