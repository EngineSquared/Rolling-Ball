#pragma once

#include "core/Core.hpp"

namespace Game {
    ES::Engine::Entity SpawnPlayer(ES::Engine::Core &core);
    void RespawnPlayer(ES::Engine::Core &core);
}