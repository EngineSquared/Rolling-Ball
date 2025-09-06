#include "PlayerEvents.hpp"

#include "JoltPhysics.hpp"
#include "FinishSegment.hpp"
#include "Player.hpp"
#include "Scene.hpp"

void Game::PlayerEvents(ES::Engine::Core &core)
{
    bool needsToFinish = false;
    core.GetRegistry()
        .view<Game::Player>()
        .each([&](auto, auto &player) {
        // TODO: should use a sensor instead, this may be buggy
        if (player.finishContacts > 0) {
            player.finishContacts = 0;
            needsToFinish = true;
        }
    });
    core.GetRegistry()
        .view<Game::Finish>()
        .each([&](auto, auto &finish) {
            if (needsToFinish) {
                finish.OnFinish(core);
            }
        });
}