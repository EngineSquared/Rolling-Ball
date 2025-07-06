#include "Scene.hpp"

#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include <variant>

using namespace ES::Plugin;

namespace Game
{
    class Option : public Scene::Utils::AScene {
        private:
            ES::Engine::Entity playButton;
      public:
      Option() : Scene::Utils::AScene() {}
    
      protected:
        void _onCreate(ES::Engine::Core &core) final
        {
            _core = &core;
            // MainMenuButton(core);
            // FullScreenButton(core);
        }
    
        void _onDestroy(ES::Engine::Core &) final
        {
            RemoveEntities();
        }
    private:
        // void MainMenuButton(ES::Engine::Core &core)
        // {
        //     auto buttonEntity = CreateEntity();
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "2DDefault");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonMainMenu");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core);
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 64.f, static_cast<float>(size.y) / 2.f - 16.f, 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("main_menu");
        //         std::cout << "Main Menu" << std::endl;
        //     };
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::BLUE_COLOR,
        //                                                         .hoverColor   = ES::Plugin::Colors::Utils::RED_COLOR,
        //                                                         .pressedColor = ES::Plugin::Colors::Utils::GREEN_COLOR};
        // }

        // void FullScreenButton(ES::Engine::Core &core)
        // {
        //     auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
    	// 	textureManager.Add(entt::hashed_string{"flNormalButton"}, "asset/textures/FullscreenNormal.png");
    	// 	textureManager.Add(entt::hashed_string{"flHoverButton"}, "asset/textures/FullscreenHover.png");
    	// 	textureManager.Add(entt::hashed_string{"flPressedButton"}, "asset/textures/FullscreenPressed.png");
        //     auto buttonEntity = CreateEntity();
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "sprite");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonFullScreen");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core, ES::Plugin::Colors::Utils::WHITE_COLOR);
        //     sprite.rect.size = glm::vec2(32.f, 32.f);
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 16.f, static_cast<float>(size.y) / 2.f - 64.f, 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "flNormalButton");
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(32.f, 32.f));
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.GetResource<Window::Resource::Window>().ToggleFullscreen();
        //     };
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::Image{
        //             .normal = ES::Plugin::OpenGL::Component::TextureHandle("flNormalButton"),
        //             .hover  = ES::Plugin::OpenGL::Component::TextureHandle("flHoverButton"),
        //             .pressed = ES::Plugin::OpenGL::Component::TextureHandle("flPressedButton")};
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
