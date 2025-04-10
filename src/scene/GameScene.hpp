#include "Scene.hpp"

#include "PlayerJump.hpp"
#include "PlayerMovement.hpp"
#include "PlayerEvents.hpp"
#include "PointCameraToPlayer.hpp"
#include "SpawnPlayer.hpp"
#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include "Generator.hpp"
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
			GenerateAndInstantiateTerrain(core);
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                Game::PlayerJump
            );
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                Game::PlayerEvents
            );
            core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
                Game::PointCameraToPlayer,
                Game::PlayerMovement
            );
            Game::SpawnPlayer(core);
        }

        void _onDestroy(ES::Engine::Core &) final
        {
            
        }
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
