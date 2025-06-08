
#include "DisplayTime.hpp"
#include "OpenGL.hpp"
#include "UI.hpp"
#include "UIResource.hpp"
#include "Time.hpp"
#include <fmt/core.h>

namespace Game
{
    void AddTimeDisplay(ES::Engine::Core &core)
    {
        core.GetResource<ES::Plugin::OpenGL::Resource::FontManager>().Add(
            entt::hashed_string("tomorrow"),
            ES::Plugin::OpenGL::Utils::Font("asset/font/Tomorrow-Medium.ttf", 32)
        );

        auto timeElapsedText = ES::Engine::Entity::Create(core);

        core.GetResource<ES::Plugin::UI::Resource::UIResource>().UpdateInnerContent("title", "Time elapsed: 0.0s");
    
        timeElapsedText.AddComponent<ES::Plugin::UI::Component::Text>(core, "Time elapsed: 0.0s", glm::vec2(10.0f, 10.0f), 1.0f, ES::Plugin::Colors::Utils::WHITE_COLOR);
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::FontHandle>(core, "tomorrow");
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "textDefault");
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::TextHandle>(core, "timeElapsedText");
    }

    void UpdateTextTime(ES::Engine::Core &core)
    {
        float ts = core.GetResource<Game::Time>().ts;
        auto &uiResource = core.GetResource<ES::Plugin::UI::Resource::UIResource>();

        core.GetRegistry()
            .view<ES::Plugin::OpenGL::Component::TextHandle, ES::Plugin::UI::Component::Text>()
            .each([&ts, &uiResource](auto, auto &textHandle, auto &text) {
                if (textHandle.name == "timeElapsedText")
                {
                    text.text = fmt::format("Time elapsed: {:.2f}s", ts);
                    uiResource.UpdateInnerContent("title", fmt::format("Time elapsed: {:.2f}s", ts));
                }
            });
    }
}
