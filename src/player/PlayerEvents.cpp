#include "PlayerEvents.hpp"

#include "PhysicsManager.hpp"
#include "RigidBody3D.hpp"
#include "FinishSegment.hpp"
#include "Player.hpp"
#include "SceneManager.hpp"

void Game::EntityTouchesFinish(ES::Engine::Core &core, JPH::Body *player)
{
    // Do we really need to check that ?
    if (player == nullptr)
        return;

    // TODO: such logic should be implemented in ESQ, not specific to the game
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    core.GetRegistry()
        .view<Game::Finish, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&physicsSystem, &player, &core](Game::Finish &finish, auto &rigidBody) {
        if (rigidBody.body == nullptr) {
            return;
        }

        if (physicsSystem.WereBodiesInContact(player->GetID(), rigidBody.body->GetID())) {
            finish.OnFinish(core);
        }
    });
}

void Game::PlayerEvents(ES::Engine::Core &core)
{
    core.GetRegistry()
        .view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto, auto &, auto &rigidBody) {
            EntityTouchesFinish(core, rigidBody.body);
    });
}