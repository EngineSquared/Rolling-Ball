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
            
            core.GetResource<UI::Resource::UIResource>().InitDocument("asset/ui/option-menu/option-menu.rml");
            core.GetResource<UI::Resource::UIResource>().AttachEventHandlers("main-menu-btn", "click", [&core](const std::string &event, const std::string &elementId) {
                if (elementId == "main-menu-btn" && event == "click") {
                    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("main_menu");
                    std::cout << "Switching to main menu scene" << std::endl;
                }
            });
            core.GetResource<UI::Resource::UIResource>().AttachEventHandlers("fullscreen-btn", "click", [&core](const std::string &event, const std::string &elementId) {
                if (elementId == "fullscreen-btn" && event == "click") {
                    core.GetResource<Window::Resource::Window>().ToggleFullscreen();
                }
            });
        }
    
        void _onDestroy(ES::Engine::Core &) final
        {
            RemoveEntities();
        }
    private:

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
