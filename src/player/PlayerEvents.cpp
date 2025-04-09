#include "PlayerEvents.hpp"

#include "PhysicsManager.hpp"
#include "RigidBody3D.hpp"
#include "FinishSegment.hpp"
#include "Player.hpp"

bool Game::EntityTouchesFinish(ES::Engine::Core &core, JPH::Body *player)
{
    if (player == nullptr)
        return false;

    // TODO: such logic should be implemented in ESQ, not specific to the game
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();
    bool touchesTerrain = false;

    core.GetRegistry()
        .view<Game::Finish, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto, auto &rigidBody) {
        if (rigidBody.body == nullptr) {
            return false;
        }

        if (physicsSystem.WereBodiesInContact(player->GetID(), rigidBody.body->GetID())) {
            touchesTerrain = true;
        }
        
    });

    return touchesTerrain;
}

void Game::PlayerEvents(ES::Engine::Core &core)
{
    core.GetRegistry()
        .view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto entity, auto &player, auto &rigidBody) {
        if (EntityTouchesFinish(core, rigidBody.body))
            std::cout << "Finished" << std::endl;  // Will be replaced by scene update
    });
}