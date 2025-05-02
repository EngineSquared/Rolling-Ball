#pragma once

#include "Engine.hpp"
#include "JoltPhysics.hpp"
#include "Player.hpp"

namespace Game {
    void ApplyJumpImpulse(ES::Engine::Core &core, const JPH::Body *body, Player &player);
} // namespace Game