#pragma once

#include "Core.hpp"
#include "OpenGL.hpp"
#include "UI.hpp"
#include "Time.hpp"

namespace Game
{
    void AddTimeDisplay(ES::Engine::Core &core)
    {
        core.GetResource<ES::Plugin::OpenGL::Resource::FontManager>().Add(
            entt::hashed_string("tomorrow"),
            ES::Plugin::OpenGL::Utils::Font("asset/font/Tomorrow-Medium.ttf", 32)
        );

        auto timeElapsedText = ES::Engine::Entity::Create(core);
    
        timeElapsedText.AddComponent<ES::Plugin::UI::Component::Text>(core, ES::Plugin::UI::Component::Text("Time elapsed: 0.0s", glm::vec2(10.0f, 10.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)));
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::FontHandle>(core, ES::Plugin::OpenGL::Component::FontHandle("tomorrow"));
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, ES::Plugin::OpenGL::Component::ShaderHandle("textDefault"));
        timeElapsedText.AddComponent<ES::Plugin::OpenGL::Component::TextHandle>(core, ES::Plugin::OpenGL::Component::TextHandle("timeElapsedText"));
    }

    void UpdateTextTime(ES::Engine::Core &core)
    {
        float ts = core.GetResource<Game::Time>().ts;

        core.GetRegistry()
            .view<ES::Plugin::OpenGL::Component::TextHandle, ES::Plugin::UI::Component::Text>()
            .each([&ts](auto, auto &textHandle, auto &text) {
                if (textHandle.name == "timeElapsedText")
                {
                    text.text = fmt::format("Time elapsed: {:.2f}s", ts);
                }
            });
    }
}