#include "Scene.hpp"

#include "AddPlayerJumpCallback.hpp"
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
#include "Events.hpp"
#include "SoundManager.hpp"
#include "Window.hpp"
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

            // Avoid trying to access the Finish segment on the last entity if a new segment has been added
            // This condition only applies when segments are added or removed from the code
            if (!entities.back().HasComponents<Game::Finish>(core)) {
                ES::Utils::Log::Warn("Last entity between save file and registry is not a Finish Segment");
                for (auto &entity : entities) {
                    if (entity.HasComponents<Game::Finish>(core)) {
                        entity.GetComponents<Game::Finish>(core).OnFinish = [&core](ES::Engine::Core &c) {
                            c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_second_level");
                        };
                    }
                }
            } else {
                entities.back().GetComponents<Game::Finish>(core).OnFinish = [&core](ES::Engine::Core &c) {
                    c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_second_level");
                };
            }
            AddTimeDisplay(core);
            _entitiesToKill.insert(_entitiesToKill.end(), entities.begin(), entities.end());
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                Game::UpdateTime,
                Game::UpdateTextTime,
                Game::PlayerEvents,
                Game::MoveSegmentsSideway,
                Game::MoveSegmentsSquish
            );
            Game::AddPlayerJumpCallback(core);
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

            DebugTexture(core);

            auto &soundManager = core.GetResource<ES::Plugin::Sound::Resource::SoundManager>();
            soundManager.SetVolume("ambient_music", 0.4f);
            soundManager.SetLoopPoints("ambient_music", 10.0f, 131.0f);
            soundManager.Play("ambient_music");

            AddLights(core, "default");
            AddLights(core, "texture");
            AddLights(core, "textureShadow");            
        }

        void AddLights(ES::Engine::Core &core, const std::string &shaderName)
        {
            float nbr_lights = 5.f;
            float scale = 2.f * glm::pi<float>() / nbr_lights;

            ES::Engine::Entity ambient_light = core.CreateEntity();
            ambient_light.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
            auto &am_transform = ambient_light.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            ambient_light.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::AMBIENT, glm::vec3(0.0f, 0.8f, 0.8f));
            am_transform.position = glm::vec3(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f));

            ES::Engine::Entity light_1 = core.CreateEntity();
            light_1.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
            auto &transform_1 = light_1.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            light_1.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.0f, 0.0f, 0.8f));
            transform_1.position = glm::vec3(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f));

            ES::Engine::Entity light_2 = core.CreateEntity();
            light_2.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
            auto &transform_2 = light_2.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            light_2.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.8f, 0.0f, 0.0f));
            transform_2.position = glm::vec3(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f));

            ES::Engine::Entity light_3 = core.CreateEntity();
            light_3.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
            auto &transform_3 = light_3.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            light_3.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.0f, 0.8f, 0.0f));
            transform_3.position = glm::vec3(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f));

            ES::Engine::Entity light_4 = core.CreateEntity();
            light_4.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
            auto &transform_4 = light_4.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            light_4.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.8f, 0.8f, 0.8f));
            transform_4.position = glm::vec3(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f));

            _entitiesToKill.insert(_entitiesToKill.end(), {ambient_light, light_1, light_2, light_3, light_4});
        }

        void _onDestroy(ES::Engine::Core &core) final
        {
            core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Stop("ambient_music");
            for (auto entity : _entitiesToKill) {
                if (entity.IsValid()) {
                    entity.Destroy(core);
                }
            }
        }
    
        inline void DebugTexture(ES::Engine::Core &core)
        {
            auto azeazez = ES::Engine::Entity::Create(core);
            azeazez.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "sprite");
            azeazez.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "dsfqdfqfdgsfdgsf");
            auto &sprite = azeazez.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core, ES::Plugin::Colors::Utils::WHITE_COLOR);
            sprite.rect.size = glm::vec2(1280.f / 4.f, 720.f / 4.f);
            auto &tr = azeazez.AddComponent<ES::Plugin::Object::Component::Transform>(core);
            auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
            glm::ivec2 size = window.GetSize();
            tr.position = glm::vec3(static_cast<float>(size.x) / 4.f * 3.f, 0.f, 0.f);
            azeazez.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "depthMap");
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

                AddLights(core, "default");
                AddLights(core, "texture");
                core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("ambient_music");
            }

            void AddLights(ES::Engine::Core &core, const std::string &shaderName)
            {
                float nbr_lights = 5.f;
                float scale = 2.f * glm::pi<float>() / nbr_lights;

                ES::Engine::Entity ambient_light = core.CreateEntity();
                ambient_light.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
                auto &am_transform = ambient_light.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                ambient_light.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::AMBIENT, glm::vec3(0.0f, 0.8f, 0.8f));
                am_transform.position = glm::vec3(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f));

                ES::Engine::Entity light_1 = core.CreateEntity();
                light_1.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
                auto &transform_1 = light_1.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                light_1.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.0f, 0.0f, 0.8f));
                transform_1.position = glm::vec3(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f));

                ES::Engine::Entity light_2 = core.CreateEntity();
                light_2.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
                auto &transform_2 = light_2.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                light_2.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.8f, 0.0f, 0.0f));
                transform_2.position = glm::vec3(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f));

                ES::Engine::Entity light_3 = core.CreateEntity();
                light_3.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
                auto &transform_3 = light_3.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                light_3.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.0f, 0.8f, 0.0f));
                transform_3.position = glm::vec3(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f));

                ES::Engine::Entity light_4 = core.CreateEntity();
                light_4.AddComponent<OpenGL::Component::ShaderHandle>(core, shaderName);
                auto &transform_4 = light_4.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                light_4.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::Type::POINT, glm::vec3(0.8f, 0.8f, 0.8f));
                transform_4.position = glm::vec3(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f));

                _entitiesToKill.insert(_entitiesToKill.end(), {ambient_light, light_1, light_2, light_3, light_4});
            }

            void _onDestroy(ES::Engine::Core &core) final
            {
                core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Stop("ambient_music");
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
