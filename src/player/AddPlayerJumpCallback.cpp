#include "AddPlayerJumpCallback.hpp"

#include "Camera.hpp"
#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "Player.hpp"
#include "InputUtils.hpp"
#include "PhysicsManager.hpp"
#include "Input.hpp"

#include "Terrain.hpp"

static bool PlayerTouchesTerrain(ES::Engine::Core &core, const JPH::BodyID &player)
{
    // TODO: such logic should be implemented in ESQ, not specific to the game
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    bool touchesTerrain = false;

    core.GetRegistry()
    .view<Game::TerrainPiece, ES::Plugin::Physics::Component::RigidBody3D>()
    .each([&](auto, auto &rigidBody) {
        if (rigidBody.body == nullptr) {
            return;
        }

        if (physicsSystem.WereBodiesInContact(player, rigidBody.body->GetID())) {
            touchesTerrain = true;
        }
    });

    return touchesTerrain;
}

static void ApplyJumpImpulse(ES::Engine::Core &core, const JPH::Body *body, const Game::Player &player)
{
    if (body == nullptr) {
        return;
    }
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    if (PlayerTouchesTerrain(core, body->GetID())) 
        bodyInterface.AddImpulse(body->GetID(), JPH::Vec3(0.0f, player.jumpImpulse, 0.0f));
}

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
