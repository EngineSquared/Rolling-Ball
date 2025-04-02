#include "PlayerJump.hpp"

// TODO: the "Body" include should be in ESQ, not here
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "Camera.hpp"
#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "Player.hpp"
#include "InputManager.hpp"
#include "PhysicsManager.hpp"

void Game::PlayerJump(ES::Engine::Core &core)
{
    auto &input = core.GetResource<ES::Plugin::Input::Resource::InputManager>();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    // TODO: only jump if player is on the floor
    auto ApplyJumpImpulse = [&](auto &body, Game::Player &player) {
        if (body == nullptr) {
            return;
        }

        if (input.IsKeyPressed(GLFW_KEY_SPACE) && player.CanJump()) {
            bodyInterface.AddImpulse(body->GetID(), JPH::Vec3(0.0f, player.jumpImpulse, 0.0f));
            player.Jump();
        }
    };

    core.GetRegistry().view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>().each([&](auto entity, auto &player, auto &rigidBody) {
        ApplyJumpImpulse(rigidBody.body, player);
    });

    core.GetRegistry().view<Game::Player, ES::Plugin::Physics::Component::SoftBody3D>().each([&](auto entity, auto &player, auto &softBody) {
        ApplyJumpImpulse(softBody.body, player);
    });
}