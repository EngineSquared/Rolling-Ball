#pragma once

#include "Core.hpp"

namespace Game
{
void LoadTextureSpriteShader(ES::Engine::Core &core)
{
	// This "using" allow to use "_hs" compile time hashing for strings
	using namespace entt;
	const std::string vertexShader = "asset/shader/sprite/sprite.vs";
	const std::string fragmentShader = "asset/shader/sprite/sprite.fs";
	auto &shaderManager = core.GetResource<OpenGL::Resource::ShaderManager>();
    OpenGL::Utils::ShaderProgram &sp = shaderManager.Add("sprite"_hs);
    sp.Create();
    sp.initFromFiles(vertexShader, fragmentShader);
    sp.addUniform("texture0");
	
    sp.addUniform("color");
    sp.addUniform("model");
    sp.addUniform("projection");
}
}
