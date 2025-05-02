#include "PlayerJumpController.hpp"

#include "Input.hpp"
#include "JumpImpulse.hpp"

#include <chrono>

static constexpr float JUMP_DELAY = 0.05f;

void Game::PlayerJumpController(ES::Engine::Core &core)
{
    using Clock = std::chrono::steady_clock;
    static auto lastJump = Clock::now();

    auto now = Clock::now();
    auto elapsed = std::chrono::duration<float>(now - lastJump).count();

    if (elapsed < JUMP_DELAY) {
        return;
    }

    try {
        auto buttons = ES::Plugin::Input::Utils::GetJoystickButtons(0);

        if (buttons.size() < 1) {
            return;
        }

        if (buttons[0] > 0.5f) {
            lastJump = now;

            core.GetRegistry()
                .view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>()
                .each([&core](auto, auto &player, auto &rigidBody) {
                    ApplyJumpImpulse(core, rigidBody.body, player);
                });

            core.GetRegistry()
                .view<Game::Player, ES::Plugin::Physics::Component::SoftBody3D>()
                .each([&core](auto, auto &player, auto &softBody) {
                    ApplyJumpImpulse(core, softBody.body, player);
                });
        }
    } catch (const ES::Plugin::Input::InputError &e) {
        return;
    }
}
