#pragma once

#include "Engine.hpp"
#include "Terrain.hpp"
#include "Save.inl"

namespace Game
{
    constexpr std::array<std::byte, 5> SAVEFILE_MAGIC_HEADER = {
        std::byte{0x45}, std::byte{0x53}, std::byte{0x51}, std::byte{0x53}, std::byte{0x46}
    };
    const std::string SAVE_FILENAME = "save.esq";

    void RetrieveSaveGameState(ES::Engine::Core &core);
    void SaveGameState(ES::Engine::Core &core);
    bool TerrainExists(ES::Engine::Core &core);
} // namespace Game
