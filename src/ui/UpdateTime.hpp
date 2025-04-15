#pragma once

#include "Core.hpp"
#include "Time.hpp"

namespace Game
{
    void UpdateTime(ES::Engine::Core& core)
    {
        core.GetResource<Game::Time>().ts += core.GetScheduler<ES::Engine::Scheduler::Update>().GetDeltaTime();
    }
}