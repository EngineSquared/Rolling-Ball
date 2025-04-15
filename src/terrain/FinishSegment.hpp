#pragma once

#include "JoltPhysics.hpp"
#include "Engine.hpp"

namespace Game
{
    struct Finish {
        std::function<void(ES::Engine::Core &)> OnFinish = nullptr;
    };
} // namespace Game
