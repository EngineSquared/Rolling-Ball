#include "JumpImpulse.hpp"

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

void Game::ApplyJumpImpulse(ES::Engine::Core &core, const JPH::Body *body, Game::Player &player)
{
    if (body == nullptr) {
        return;
    }
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    if (PlayerTouchesTerrain(core, body->GetID()))
        bodyInterface.AddImpulse(body->GetID(), JPH::Vec3(0.0f, player.jumpImpulse, 0.0f));
}
