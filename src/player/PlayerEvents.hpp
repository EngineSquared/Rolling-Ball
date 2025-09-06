#pragma once

#include "core/Core.hpp"
#include "JoltPhysics.hpp"

namespace Game {
    void EntityTouchesFinish(ES::Engine::Core &core, JPH::Body *body);
    void PlayerEvents(ES::Engine::Core &core);
} // namespace Game
