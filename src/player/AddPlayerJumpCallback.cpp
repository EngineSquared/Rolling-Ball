#include "AddPlayerJumpCallback.hpp"

#include "Camera.hpp"
#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "Player.hpp"
#include "InputUtils.hpp"
#include "PhysicsManager.hpp"
#include "Input.hpp"
#include "JumpImpulse.hpp"

#include "Terrain.hpp"

void Game::AddPlayerJumpCallback(ES::Engine::Core &core)
{
    auto &inputManager = core.GetResource<ES::Plugin::Input::Resource::InputManager>();
    inputManager.RegisterKeyCallback([](ES::Engine::Core &cbCore, int key, int, int action, int) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            cbCore.GetRegistry()
                .view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>()
                .each([&cbCore](auto, auto &player, auto &rigidBody) {
                    ApplyJumpImpulse(cbCore, rigidBody.body, player);
            });

            cbCore.GetRegistry()
                .view<Game::Player, ES::Plugin::Physics::Component::SoftBody3D>()
                .each([&cbCore](auto, auto &player, auto &softBody) {
                    ApplyJumpImpulse(cbCore, softBody.body, player);
            });
        }
    });
}
