#include "Save.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "GameScene.hpp"

static bool CreateSaveFile(const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;
    return true;
}

bool Game::TerrainExists(ES::Engine::Core &core)
{
    if (!core.GetRegistry().view<Game::Terrain>().empty())
        return true;
    return false;
}

void Game::RetrieveSaveGameState(ES::Engine::Core &)
{
    if (!std::filesystem::exists(SAVE_FILENAME))
        return;
    std::fstream saveFile(SAVE_FILENAME, std::ios::binary | std::ios::in);
    if (!saveFile.is_open()) {
        ES::Utils::Log::Error("Could not open save file");
        return;
    }
    std::array<std::byte, 5> fileHeader;
    saveFile.read(reinterpret_cast<char*>(fileHeader.data()), fileHeader.size());
    if (fileHeader != SAVEFILE_MAGIC_HEADER) {
        ES::Utils::Log::Error("Invalid save file: magic header mismatch");
        saveFile.close();
        return;
    }

    Game::Terrain terrain;
    saveFile.read(reinterpret_cast<char*>(&terrain.segmentCount), sizeof(terrain.segmentCount));
    saveFile.read(reinterpret_cast<char*>(&terrain.segmentsPositionOffset), sizeof(terrain.segmentsPositionOffset));
    saveFile.read(reinterpret_cast<char*>(&terrain.segmentsGapPositionOffset), sizeof(terrain.segmentsGapPositionOffset));
    saveFile.read(reinterpret_cast<char*>(&terrain.segmentsScale), sizeof(terrain.segmentsScale));
    saveFile.read(reinterpret_cast<char*>(&terrain.segmentsRotation), sizeof(terrain.segmentsRotation));
    saveFile.read(reinterpret_cast<char*>(&terrain.rng), sizeof(terrain.rng));

    for (int i = 0; i < terrain.segmentCount + 1; ++i) {
        Game::TerrainPiece piece;
        saveFile.read(reinterpret_cast<char*>(&piece.type), sizeof(piece.type));
        saveFile.read(reinterpret_cast<char*>(&piece.position), sizeof(piece.position));
        saveFile.read(reinterpret_cast<char*>(&piece.scale), sizeof(piece.scale));
        saveFile.read(reinterpret_cast<char*>(&piece.rotationAngle), sizeof(piece.rotationAngle));
        terrain.pieces.push_back(piece);
    }
    saveFile.close();
    loadedTerrain = terrain;
    ES::Utils::Log::Info("Loaded terrain from save successfully");
}

void Game::SaveGameState(ES::Engine::Core &core)
{
    if (!TerrainExists(core))
        return; // Do not save on the main menu
    if (!std::filesystem::exists(SAVE_FILENAME)) {
        if (!CreateSaveFile(SAVE_FILENAME)) {
            ES::Utils::Log::Error("Could not save the game, failed to create save file");
            return;
        }
        ES::Utils::Log::Info("Save file created successfully");
    }
    std::fstream saveFile(SAVE_FILENAME, std::ios::binary | std::ios::out | std::ios::trunc);

    if (!saveFile.is_open()) {
        ES::Utils::Log::Error("Failed to open save file.");
        return;
    }
    core.GetRegistry().view<Game::Terrain>().each(
        [&](const auto &terrain) {
            saveFile.write(reinterpret_cast<const char*>(SAVEFILE_MAGIC_HEADER.data()), SAVEFILE_MAGIC_HEADER.size());
            saveFile.write(reinterpret_cast<const char*>(&terrain.segmentCount), sizeof(terrain.segmentCount));
            saveFile.write(reinterpret_cast<const char*>(&terrain.segmentsPositionOffset), sizeof(terrain.segmentsPositionOffset));
            saveFile.write(reinterpret_cast<const char*>(&terrain.segmentsGapPositionOffset), sizeof(terrain.segmentsGapPositionOffset));
            saveFile.write(reinterpret_cast<const char*>(&terrain.segmentsScale), sizeof(terrain.segmentsScale));
            saveFile.write(reinterpret_cast<const char*>(&terrain.segmentsRotation), sizeof(terrain.segmentsRotation));
            saveFile.write(reinterpret_cast<const char*>(&terrain.rng), sizeof(terrain.rng));
            for (const auto &piece : terrain.pieces) {
                // Writing each fields one by one to avoid struct padding
                saveFile.write(reinterpret_cast<const char*>(&piece.type), sizeof(piece.type));
                saveFile.write(reinterpret_cast<const char*>(&piece.position), sizeof(piece.position));
                saveFile.write(reinterpret_cast<const char*>(&piece.scale), sizeof(piece.scale));
                saveFile.write(reinterpret_cast<const char*>(&piece.rotationAngle), sizeof(piece.rotationAngle));
            }
        }
    );
    saveFile.close();
    ES::Utils::Log::Info("Game saved");
}