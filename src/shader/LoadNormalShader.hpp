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

    sp.addUniform("NumberLights");
    sp.addSSBO("LightBuffer", 0, sizeof(ES::Plugin::OpenGL::Utils::LightInfo));
    sp.addUniform("Material.Ka");
    sp.addUniform("Material.Kd");
    sp.addUniform("Material.Ks");
    sp.addUniform("Material.Shiness");

    sp.addUniform("CamPos");

	sp.use();
    glUniform3fv(sp.uniform("CamPos"), 1, glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));
	sp.disable();

    float nbr_lights = 5.f;
    float scale = 2.f * glm::pi<float>() / nbr_lights;

    ES::Engine::Entity ambient_light = core.CreateEntity();
    ambient_light.AddComponent<OpenGL::Component::ShaderHandle>(core, "normal");
    auto &am_transform = ambient_light.AddComponent<Object::Component::Transform>(core, glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    ambient_light.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::TYPE::AMBIENT, glm::vec3(0.0f, 0.8f, 0.8f));
    am_transform.position = glm::vec3(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f));

    ES::Engine::Entity light_1 = core.CreateEntity();
    light_1.AddComponent<OpenGL::Component::ShaderHandle>(core, "normal");
    auto &transform_1 = light_1.AddComponent<Object::Component::Transform>(core, glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    light_1.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::TYPE::POINT, glm::vec3(0.0f, 0.0f, 0.8f));
    transform_1.position = glm::vec3(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f));

    ES::Engine::Entity light_2 = core.CreateEntity();
    light_2.AddComponent<OpenGL::Component::ShaderHandle>(core, "normal");
    auto &transform_2 = light_2.AddComponent<Object::Component::Transform>(core, glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    light_2.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::TYPE::POINT, glm::vec3(0.8f, 0.0f, 0.0f));
    transform_2.position = glm::vec3(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f));

    ES::Engine::Entity light_3 = core.CreateEntity();
    light_3.AddComponent<OpenGL::Component::ShaderHandle>(core, "normal");
    auto &transform_3 = light_3.AddComponent<Object::Component::Transform>(core, glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    light_3.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::TYPE::POINT, glm::vec3(0.0f, 0.8f, 0.0f));
    transform_3.position = glm::vec3(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f));

    ES::Engine::Entity light_4 = core.CreateEntity();
    light_4.AddComponent<OpenGL::Component::ShaderHandle>(core, "normal");
    auto &transform_4 = light_4.AddComponent<Object::Component::Transform>(core, glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    light_4.AddComponent<OpenGL::Component::Light>(core, OpenGL::Component::Light::TYPE::POINT, glm::vec3(0.8f, 0.8f, 0.8f));
    transform_4.position = glm::vec3(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f));
}
}