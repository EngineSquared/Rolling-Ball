#pragma once

#include "Core.hpp"

namespace Game
{
void LoadTextureShader(ES::Engine::Core &core)
{
	// This "using" allow to use "_hs" compile time hashing for strings
	using namespace entt;
	const std::string vertexShader = "asset/shader/texture/texture.vs";
	const std::string fragmentShader = "asset/shader/texture/texture.fs";
	auto &shaderManager = core.GetResource<OpenGL::Resource::ShaderManager>();
    OpenGL::Utils::ShaderProgram &sp = shaderManager.Add("texture"_hs);
    sp.Create();
    sp.InitFromFiles(vertexShader, fragmentShader);
    sp.AddUniform("texture0");
    sp.AddUniform("lightSpaceMatrix");
    sp.AddUniform("shadowMap");
	sp.AddUniform("MVP");
    sp.AddUniform("ModelMatrix");
    sp.AddUniform("NormalMatrix");

    sp.AddUniform("NumberLights");
    sp.AddSSBO("LightBuffer", 0, sizeof(ES::Plugin::OpenGL::Utils::LightInfo));
    sp.AddUniform("Material.Ka");
    sp.AddUniform("Material.Kd");
    sp.AddUniform("Material.Ks");
    sp.AddUniform("Material.Shiness");

    sp.AddUniform("CamPos");

	sp.Use();
    glUniform3fv(sp.GetUniform("CamPos"), 1, glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));
	sp.Disable();
}
}
