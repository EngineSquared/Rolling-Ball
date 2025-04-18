#pragma once

#include "Engine.hpp"
#include "Terrain.hpp"

namespace Game
{
    SegmentMetrics GetSegmentMetrics(TerrainType type, const glm::vec3 &scale, const glm::quat &rotation);
    TerrainType GetRandomTerrainType(std::mt19937 &rng);
    std::vector<ES::Engine::Entity> InstantiateLoadedTerrain(ES::Engine::Core &core, const Terrain &terrain);
    std::vector<ES::Engine::Entity> GenerateAndInstantiateTerrain(ES::Engine::Core &core);
    void GenerateTerrain(Terrain &terrain);
    ES::Engine::Entity CreateTerrainPiece(ES::Engine::Core &core, const TerrainPiece &piece);

    static std::unordered_map<TerrainType, SegmentMetrics> cachedMetrics;
} // namespace Game
