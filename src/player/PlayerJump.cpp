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

// TODO: this is dirty
#include "../terrain/Terrain.hpp"

static bool PlayerTouchesTerrain(ES::Engine::Core &core, const JPH::BodyID &player)
{
    // TODO: such logic should be implemented in ESQ, not specific to the game
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    bool touchesTerrain = false;

    core.GetRegistry()
        .view<Game::Terrain, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto entity, auto &rigidBody) {
        if (rigidBody.body == nullptr) {
            return;
        }

        if (physicsSystem.WereBodiesInContact(player, rigidBody.body->GetID())) {
            touchesTerrain = true;
        }
        
    });

    return touchesTerrain;
}

static void ApplyJumpImpulse(ES::Engine::Core &core, JPH::Body *body, Game::Player &player)
{
    if (body == nullptr) {
        return;
    }

    auto &input = core.GetResource<ES::Plugin::Input::Resource::InputManager>();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    if (input.IsKeyPressed(GLFW_KEY_SPACE) && PlayerTouchesTerrain(core, body->GetID())) {
        bodyInterface.AddImpulse(body->GetID(), JPH::Vec3(0.0f, player.jumpImpulse, 0.0f));
        printf("Jump\n");
    }
}

void Game::PlayerJump(ES::Engine::Core &core)
{
    auto &input = core.GetResource<ES::Plugin::Input::Resource::InputManager>();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    core.GetRegistry()
        .view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto entity, auto &player, auto &rigidBody) {
        ApplyJumpImpulse(core, rigidBody.body, player);
    });

    core.GetRegistry()
        .view<Game::Player, ES::Plugin::Physics::Component::SoftBody3D>()
        .each([&](auto entity, auto &player, auto &softBody) {
        ApplyJumpImpulse(core, softBody.body, player);
    });
}