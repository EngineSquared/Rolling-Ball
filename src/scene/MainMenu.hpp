#include "Scene.hpp"

#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include <variant>

using namespace ES::Plugin;

namespace Game
{
    class MainMenu : public Scene::Utils::AScene {
        private:
            ES::Engine::Entity playButton;
      public:
      MainMenu() : Scene::Utils::AScene() {}
    
      protected:
        void _onCreate(ES::Engine::Core &core) final
        {
            _core = &core;
            playButton = PlayButton(core);
        }
    
        void _onDestroy(ES::Engine::Core &core) final
        {
            RemoveEntities();
        }
    private:
        inline ES::Engine::Entity PlayButton(ES::Engine::Core &core)
        {
            auto buttonEntity = CreateEntity();
            buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "2DDefault");
            buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonPlay");
            auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core);
            sprite.rect.size = glm::vec2(128.f, 32.f);
            auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
            auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
            int width, height;
            window.GetWindowSize(width, height);
            tr.position = glm::vec3(static_cast<float>(width) / 2.f - 64.f, static_cast<float>(height) / 2.f - 16.f, 0.f);
            buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
            auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
            buttonComp.onClick = [&](ES::Engine::Core &c) {
                c.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("game_first_level");
            };
            buttonComp.displayType =
                ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::BLUE_COLOR,
                                                                .hoverColor   = ES::Plugin::Colors::Utils::RED_COLOR,
                                                                .pressedColor = ES::Plugin::Colors::Utils::GREEN_COLOR};
            return buttonEntity;
        }

        // TODO: those kind of entity management should be added into scene class to allow to easely create scene contextual entities
        // (RemoveEntities, CreateEntity, _entitiesToKill, _core)
        void RemoveEntities()
        {
            for (auto &entity : _entitiesToKill)
            {
                _core->KillEntity(entity);
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
