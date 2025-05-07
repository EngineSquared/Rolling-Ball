#include "InitPlayerContactCallback.hpp"

#include "PhysicsManager.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "FinishSegment.hpp"

void Game::InitPlayerContactCallback(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>()
        .AddContactAddedCallback<Game::Player, Game::Terrain>(
            [](ES::Engine::Core &cbCore, ES::Engine::Entity &player, const ES::Engine::Entity &)
            {
                auto &playerComponent = player.GetComponents<Game::Player>(cbCore);
                playerComponent.terrainContacts++;
            }
        );
    core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>()
        .AddContactRemovedCallback<Game::Player, Game::Terrain>(
            [](ES::Engine::Core &cbCore, ES::Engine::Entity &player, const ES::Engine::Entity &)
            {
                auto &playerComponent = player.GetComponents<Game::Player>(cbCore);
                playerComponent.terrainContacts--;
                if (playerComponent.terrainContacts < 0) {
                    playerComponent.terrainContacts = 0;
                }
            }
        );

        core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>()
        .AddContactAddedCallback<Game::Player, Game::Finish>(
            [](ES::Engine::Core &cbCore, ES::Engine::Entity &player, const ES::Engine::Entity &)
            {
                auto &playerComponent = player.GetComponents<Game::Player>(cbCore);
                playerComponent.finishContacts++;
            }
        );
    core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>()
        .AddContactRemovedCallback<Game::Player, Game::Finish>(
            [](ES::Engine::Core &cbCore, ES::Engine::Entity &player, const ES::Engine::Entity &)
            {
                auto &playerComponent = player.GetComponents<Game::Player>(cbCore);
                playerComponent.finishContacts--;
                if (playerComponent.finishContacts < 0) {
                    playerComponent.finishContacts = 0;
                }
            }
        );
}