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

    static const std::string GetModelPathFromTerrainType(const TerrainType type) {
        static const std::unordered_map<TerrainType, std::string> modelPaths = {
            {TerrainType::Flat, "asset/models/straight.obj"},
            {TerrainType::Ramp, "asset/models/ramp.obj"},
            {TerrainType::CurveLeft, "asset/models/curve_left.obj"},
            {TerrainType::CurveRight, "asset/models/curve_right.obj"},
            {TerrainType::Wave, "asset/models/wave.obj"},
            {TerrainType::Jump, "asset/models/jump.obj"},
            {TerrainType::JumpSingle, "asset/models/jump_single.obj"},
            {TerrainType::ObstacleA, "asset/models/obstacle1.obj"},
            {TerrainType::ObstacleB, "asset/models/obstacle2.obj"},
            {TerrainType::ObstacleC, "asset/models/obstacle3.obj"},
            {TerrainType::MovingObstacleA, "asset/models/moving_obstacle1.obj"},
            {TerrainType::MovingObstacleB, "asset/models/moving_obstacle2.obj"},
            {TerrainType::Finish, "asset/models/finish.obj"},
        };
    
        auto it = modelPaths.find(type);
        if (it != modelPaths.end()) {
            return it->second;
        }
        return "asset/models/straight.obj"; // Default fallback
    }
}