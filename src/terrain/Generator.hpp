#pragma once

#include "Engine.hpp"
#include "Terrain.hpp"

namespace Game
{
    Game::TerrainType GetRandomTerrainType(std::mt19937 &rng);
    void GenerateAndInstantiateTerrain(ES::Engine::Core &core);
    void GenerateTerrain(Game::Terrain &terrain);
    ES::Engine::Entity CreateTerrainPiece(ES::Engine::Core &core, const TerrainPiece &piece);
} // namespace Game
