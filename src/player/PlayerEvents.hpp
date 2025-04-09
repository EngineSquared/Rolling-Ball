#pragma once

#include "Core.hpp"
#include "JoltPhysics.hpp"

namespace Game {
    bool EntityTouchesFinish(ES::Engine::Core &core, JPH::Body *body);
    void PlayerEvents(ES::Engine::Core &core);
} // namespace Game
