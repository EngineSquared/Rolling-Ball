#include "Scene.hpp"

#include "PlayerJump.hpp"
#include "PlayerMovement.hpp"
#include "PlayerEvents.hpp"
#include "PointCameraToPlayer.hpp"
#include "SpawnPlayer.hpp"
#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include "Player.hpp"
#include "Generator.hpp"
#include "DisplayTime.hpp"
#include <variant>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

using namespace ES::Plugin;

namespace Game
{
    class FirstLevelScene : public ES::Plugin::Scene::Utils::AScene {
    
    public:
        FirstLevelScene() : ES::Plugin::Scene::Utils::AScene() {}
    
    protected:
        void _onCreate(ES::Engine::Core &core) final
        {
			std::vector<ES::Engine::Entity> entities = GenerateAndInstantiateTerrain(core);
            AddTimeDisplay(core);
            _entitiesToKill.insert(_entitiesToKill.end(), entities.begin(), entities.end());
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                UpdateTextTime,
                Game::PlayerJump,
                Game::PlayerEvents
            );
            core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
                Game::PointCameraToPlayer,
                Game::PlayerMovement
            );
            core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
                Game::RespawnPlayer
            );
            _entitiesToKill.push_back(Game::SpawnPlayer(core));
        }

        void _onDestroy(ES::Engine::Core &core) final
        {
            for (auto entity : _entitiesToKill) {
                if (entity.IsValid()) {
                    entity.Destroy(core);
                }
            }
        }
    private:
        std::vector<ES::Engine::Entity> _entitiesToKill;
    };

    class SecondLevelScene : public ES::Plugin::Scene::Utils::AScene {
    
        public:
            SecondLevelScene() : ES::Plugin::Scene::Utils::AScene() {}

        protected:
            void _onCreate(ES::Engine::Core &core) final
            {
                GenerateAndInstantiateTerrain(core);
                Game::SpawnPlayer(core);
            }

            void _onDestroy(ES::Engine::Core &) final
            {
                
            }
        };
}
