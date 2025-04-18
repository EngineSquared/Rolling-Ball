#pragma once

#include "Core.hpp"

namespace Game
{
void LoadNormalShader(ES::Engine::Core &core)
{
	// This "using" allow to use "_hs" compile time hashing for strings
	using namespace entt;
	const std::string vertexShader = "asset/shader/normal/normal.vs";
	const std::string fragmentShader = "asset/shader/normal/normal.fs";
	auto &shaderManager = core.GetResource<OpenGL::Resource::ShaderManager>();
    OpenGL::Utils::ShaderProgram &sp = shaderManager.Add("normal"_hs);
    sp.Create();
    sp.initFromFiles(vertexShader, fragmentShader);
	sp.addUniform("MVP");
    sp.addUniform("ModelMatrix");
    sp.addUniform("NormalMatrix");

    for (int i = 0; i < 5; i++)
    {
        sp.addUniform(fmt::format("Light[{}].Position", i));
        sp.addUniform(fmt::format("Light[{}].Intensity", i));
    }
    sp.addUniform("Material.Ka");
    sp.addUniform("Material.Kd");
    sp.addUniform("Material.Ks");
    sp.addUniform("Material.Shiness");

    sp.addUniform("CamPos");

	sp.use();
    glUniform3fv(sp.uniform("CamPos"), 1, glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));
	sp.disable();

    std::array<OpenGL::Utils::Light, 5> light = {
        OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.8f)},
        OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.0f, 0.8f)},
        OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.0f, 0.0f)},
        OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.0f)},
        OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.8f, 0.8f)}
    };

    float nbr_lights = 5.f;
    float scale = 2.f * glm::pi<float>() / nbr_lights;

    light[0].position = glm::vec4(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f), 1.f);
    light[1].position = glm::vec4(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f), 1.f);
    light[2].position = glm::vec4(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f), 1.f);
    light[3].position = glm::vec4(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f), 1.f);
    light[4].position = glm::vec4(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f), 1.f);

    sp.use();
    for (int i = 0; i < 5; i++)
    {
        glUniform4fv(sp.uniform(fmt::format("Light[{}].Position", i).c_str()), 1,
                     glm::value_ptr(light[i].position));
        glUniform3fv(sp.uniform(fmt::format("Light[{}].Intensity", i).c_str()), 1,
                     glm::value_ptr(light[i].intensity));
    }
    sp.disable();
}
}