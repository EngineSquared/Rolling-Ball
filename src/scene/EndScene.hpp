#include "Scene.hpp"

#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include "Time.hpp"
#include <variant>

using namespace ES::Plugin;

namespace Game
{
    class EndScene : public Scene::Utils::AScene {
    public:
        EndScene() : Scene::Utils::AScene() {}
    
    protected:
        void _onCreate(ES::Engine::Core &core) final
        {
            _core = &core;
            // QuitButton(core);
            // DestroyElapsedTimeText(core);
            // DisplayTime(core);
        }
    
        void _onDestroy(ES::Engine::Core &) final
        {
            RemoveEntities();
        }
    private:
        void DestroyElapsedTimeText(ES::Engine::Core &core)
        {
            auto view = core.GetRegistry().view<ES::Plugin::OpenGL::Component::TextHandle, ES::Plugin::UI::Component::Text>();
            for (auto entity : view)
            {
                auto &textHandle = view.get<ES::Plugin::OpenGL::Component::TextHandle>(entity);
                if (textHandle.name == "timeElapsedText")
                {
                    ES::Engine::Entity(entity).Destroy(core);
                }
            }
        }

        // void DisplayTime(ES::Engine::Core &core)
        // {
        //     float elapsedTime = core.GetResource<Game::Time>().ts;
        //     auto timeElapsedText = ES::Engine::Entity::Create(core);
    
        //     timeElapsedText.AddComponent<ES::Plugin::UI::Component::Text>(core, fmt::format("Game Finished in: {:.2f} seconds", elapsedTime), glm::vec2(100.0f, 100.0f), 1.0f, ES::Plugin::Colors::Utils::WHITE_COLOR);
        //     timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::FontHandle>(core, "tomorrow");
        //     timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "textDefault");
        //     timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::TextHandle>(core, "timeElapsedTextEnd");
        // }

        // void QuitButton(ES::Engine::Core &core)
        // {
        //     auto buttonEntity = CreateEntity();
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "2DDefault");
        //     buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonGameOverQuit");
        //     auto &sprite = buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core);
        //     sprite.rect.size = glm::vec2(128.f, 32.f);
        //     sprite.color = ES::Plugin::Colors::Utils::BLUE_COLOR;
        //     auto &tr = buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
        //     auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
        //     glm::ivec2 size = window.GetSize();
        //     tr.position = glm::vec3(static_cast<float>(size.x) / 2.f - 64.f, static_cast<float>(size.y) / 2.f - 16.f, 0.f);
        //     buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(128.f, 32.f));
        //     auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
        //     buttonComp.onClick = [&](ES::Engine::Core &c) {
        //         c.Stop();
        //     };
        //     buttonComp.displayType =
        //         ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::BLUE_COLOR,
        //                                                         .hoverColor   = ES::Plugin::Colors::Utils::RED_COLOR,
        //                                                         .pressedColor = ES::Plugin::Colors::Utils::GREEN_COLOR};
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
