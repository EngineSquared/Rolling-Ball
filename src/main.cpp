#ifndef VULKAN
#    include <GL/glew.h>
#endif
#include "Engine.hpp"
#include "component/Transform.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"
#include "resource/Window/Window.hpp"
#include "OpenGL.hpp"
#include "Engine.hpp"
#include "Input.hpp"
#include "Sound.hpp"

#include "Sounds.hpp"
#include "SpawnPlayer.hpp"
#include "PointCameraToPlayer.hpp"
#include "PlayerMovement.hpp"
#include "InitPlayerContactCallback.hpp"
#include "Generator.hpp"
#include "Save.hpp"
#include "GameScene.hpp"
#include "MainMenu.hpp"
#include "Option.hpp"
#include "EndScene.hpp"
#include "LoadNormalShader.hpp"
#include "LoadTextureShader.hpp"
#include "LoadTextureSpriteShader.hpp"
#include "PlayerJumpController.hpp"
#include <iostream>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RenderingPipeline.hpp"

using namespace JPH; // NOT RECOMMENDED

using namespace JPH::literals;
using namespace ES::Plugin;

void UpdateTextureLightShadowShader(ES::Engine::Core &core){
	const auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();
	if (!light.enabled)
		return;
	auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"texture"});

	shaderProgram.Use();

	// Link texture to the shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, light.depthMap);
	glUniform1i(shaderProgram.GetUniform("shadowMap"), 1);

	// Link Light Space Matrix to the shader
	glUniformMatrix4fv(shaderProgram.GetUniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));

	// Link Camera Position to the shader
	glUniform3fv(shaderProgram.GetUniform("CamPos"), 1, glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));

	shaderProgram.Disable();
}

int main(void)
{
    ES::Engine::Core core;

	core.AddPlugins<Physics::Plugin, Input::Plugin, OpenGL::Plugin, UI::Plugin>();

	core.RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());
	core.RegisterResource<ES::Plugin::Sound::Resource::SoundManager>(ES::Plugin::Sound::Resource::SoundManager());

	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::FirstLevelScene>("game_first_level");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::SecondLevelScene>("game_second_level");
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::MainMenu>("main_menu");
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::Option>("option");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::EndScene>("end_scene");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("main_menu");

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(
		[](const ES::Engine::Core &) {
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
				GLsizei, const GLchar *message, const void *) {
				std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
						  << " Source: " << source
						  << ", Type: " << type
						  << ", ID: " << id
						  << ", Severity: " << severity
						  << ", Message: " << message << std::endl;
			}, nullptr);
			glfwSetErrorCallback([](int error, const char *description) {
				std::cerr << "GLFW ERROR: " << error << ": " << description << std::endl;
			});
		},
		[](ES::Engine::Core &c) {
			c.GetResource<Window::Resource::Window>().SetTitle("ES Rolling-Ball");
			c.GetResource<Window::Resource::Window>().SetSize(1280, 720);
		},
		[](ES::Engine::Core &c) {
			c.GetResource<OpenGL::Resource::Camera>().viewer.lookFrom(glm::vec3(0.0f, 10.0f, -20.0f));
		},
		[](ES::Engine::Core &c) {
			c.GetResource<Physics::Resource::PhysicsManager>().GetPhysicsSystem().OptimizeBroadPhase();
			c.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(2);
			c.GetScheduler<ES::Engine::Scheduler::FixedTimeUpdate>().SetTickRate(1.0f / 240.0f);
		},
		Game::LoadNormalShader,
		Game::LoadTextureShader,
		Game::LoadTextureSpriteShader
	);

	core.RegisterSystem<ES::Engine::Scheduler::Update>(Game::PlayerJumpController);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>(Game::InitPlayerContactCallback);
	core.RegisterSystem<ES::Engine::Scheduler::Startup>(ES::Plugin::Sound::System::InitSounds);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>(Game::RegisterGameSounds);
	core.RegisterSystem<ES::Engine::Scheduler::Startup>(Game::RetrieveSaveGameState);
	core.RegisterSystem<ES::Engine::Scheduler::Shutdown>(Game::SaveGameState);


	core.RegisterSystem<ES::Engine::Scheduler::Update>(
		ES::Plugin::Scene::System::UpdateScene,
        // ES::Plugin::UI::System::ButtonClick,
        ES::Engine::Entity::RemoveTemporaryComponents
        // ES::Plugin::UI::System::UpdateButtonState,
        // ES::Plugin::UI::System::UpdateButtonTexture
	);

	core.RegisterSystem<ES::Plugin::RenderingPipeline::RenderSetup>(
		UpdateTextureLightShadowShader
	);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&](ES::Engine::Core &c) {
		ES::Plugin::Input::Utils::PrintAvailableControllers();

		auto &inputManager = c.GetResource<Input::Resource::InputManager>();
		inputManager.RegisterKeyCallback([](ES::Engine::Core &cbCore, int key, int, int action, int) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				cbCore.Stop();
			}
		});
	});

	core.RunCore();

    return 0;
}
