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
#include "Save.hpp"
#include "DisplayTime.hpp"
#include "FinishSegment.hpp"
#include "Time.hpp"
#include "UpdateTime.hpp"
#include "OpenGL.hpp"
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
			std::vector<ES::Engine::Entity> entities;
            if (Game::loadedTerrain.has_value()) {
                entities = Game::InstantiateLoadedTerrain(core, Game::loadedTerrain.value());
            } else {
                entities = Game::GenerateAndInstantiateTerrain(core);
            }

            entities.back().GetComponents<Game::Finish>(core).OnFinish = [&core](ES::Engine::Core &c) {
                c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_second_level");
            };
            AddTimeDisplay(core);
            _entitiesToKill.insert(_entitiesToKill.end(), entities.begin(), entities.end());
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                Game::UpdateTime,
                Game::UpdateTextTime,
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
            core.RegisterResource<Game::Time>(Game::Time{0.0f});
            auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            textureManager.Add(entt::hashed_string{"default"}, "asset/textures/default.png");
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
                std::vector<ES::Engine::Entity> entities = GenerateAndInstantiateTerrain(core);
                entities.back().GetComponents<Game::Finish>(core).OnFinish = [&core](ES::Engine::Core &c) {
                    c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("end_scene");
                };
                _entitiesToKill.insert(_entitiesToKill.end(), entities.begin(), entities.end());
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
}
