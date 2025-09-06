#include "Scene.hpp"

#include "core/Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include <variant>

using namespace ES::Plugin;

namespace Game
{
    class MainMenu : public Scene::Utils::AScene {
      public:
      MainMenu() : Scene::Utils::AScene() {}
    
      protected:
        void _onCreate(ES::Engine::Core &core) final
        {
            _core = &core;
            core.GetResource<UI::Resource::UIResource>().SetFont("asset/font/Tomorrow-Medium.ttf");
            core.GetResource<UI::Resource::UIResource>().InitDocument("asset/ui/main-menu/main-menu.rml");
            core.GetResource<UI::Resource::UIResource>().AttachEventHandlers("start-game-btn", "click", [&core](const std::string &event, const std::string &elementId) {
                if (elementId == "start-game-btn" && event == "click") {
                    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_first_level");
                    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
                }
            });
            core.GetResource<UI::Resource::UIResource>().AttachEventHandlers("option-game-btn", "click", [&core](const std::string &event, const std::string &elementId) {
                if (elementId == "option-game-btn" && event == "click") {
                    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
                    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("option");
                }
            });
            core.GetResource<UI::Resource::UIResource>().AttachEventHandlers("quit-game-btn", "click", [&core](const std::string &event, const std::string &elementId) {
                if (elementId == "quit-game-btn" && event == "click") {
                    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
                    core.Stop();
                }
            });
        }
    
        void _onDestroy(ES::Engine::Core &) final
        {
            RemoveEntities();
        }
    private:
        // inline void PlayButton(ES::Engine::Core &core)
        // {
        //     auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
    	// 	textureManager.Add(entt::hashed_string{"startButton"}, "asset/textures/StartButton.png");
        //     auto buttonEntity = CreateEntity();
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "sprite");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonPlay");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core, ES::Plugin::Colors::Utils::WHITE_COLOR);
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 64.f, static_cast<float>(size.y) / 2.f + 32.f, 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "startButton");
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_first_level");
        //         c.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
        //     };
            
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::WHITE_COLOR,
        //                                                         .hoverColor   = ES::Plugin::Colors::Utils::LIGHTGRAY_COLOR,
        //                                                         .pressedColor = ES::Plugin::Colors::Utils::GRAY_COLOR};
        // }

        // inline void OptionButton(ES::Engine::Core &core)
        // {
        //     auto buttonEntity = CreateEntity();
        //     auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
    	// 	textureManager.Add(entt::hashed_string{"optionButton"}, "asset/textures/OptionButton.png");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "sprite");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonOption");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core, ES::Plugin::Colors::Utils::WHITE_COLOR);
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 64.f, static_cast<float>(size.y) / 2.f - 16.f, 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "optionButton");
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("option");
        //         c.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
        //     };
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::WHITE_COLOR,
        //                                                         .hoverColor   = ES::Plugin::Colors::Utils::LIGHTGRAY_COLOR,
        //                                                         .pressedColor = ES::Plugin::Colors::Utils::GRAY_COLOR};
        // }

        // inline void QuitButton(ES::Engine::Core &core)
        // {
        //     auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
    	// 	textureManager.Add(entt::hashed_string{"quitButton"}, "asset/textures/QuitButton.png");
        //     auto buttonEntity = CreateEntity();
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "sprite");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonQuit");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core, ES::Plugin::Colors::Utils::WHITE_COLOR);
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 64.f, static_cast<float>(size.y) / 2.f - (16.f * 2.f + 32.f), 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "quitButton");
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Play("button_click");
        //         c.Stop();
        //     };
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::WHITE_COLOR,
        //                                                         .hoverColor   = ES::Plugin::Colors::Utils::LIGHTGRAY_COLOR,
        //                                                         .pressedColor = ES::Plugin::Colors::Utils::GRAY_COLOR};
        // }

        // TODO: those kind of entity management should be added into scene class to allow to easely create scene contextual entities
        // (RemoveEntities, CreateEntity, _entitiesToKill, _core)
        void RemoveEntities()
        {
            for (auto &entity : _entitiesToKill)
            {
                entity.Destroy(*_core);
            }
            _entitiesToKill.clear();
        }

        ES::Engine::Entity CreateEntity()
        {
            auto entity = _core->CreateEntity();
            _entitiesToKill.push_back(entity);
            return entity;
        }
    private:

        ES::Engine::Core *_core;
        std::vector<ES::Engine::Entity> _entitiesToKill;
    };
}
