#ifndef VULKAN
#    include <GL/glew.h>
#endif
#include "Core.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "FixedTimeUpdate.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Sounds.hpp"
#include "Input.hpp"
#include "InitSound.hpp"

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

using namespace JPH; // NOT RECOMMENDED

using namespace JPH::literals;
using namespace ES::Plugin;

int main(void)
{
    ES::Engine::Core core;

	core.AddPlugins<Physics::Plugin, Input::Plugin, OpenGL::Plugin>();

	core.RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());
	core.RegisterResource<ES::Plugin::Sound::Resource::SoundManager>(ES::Plugin::Sound::Resource::SoundManager());

	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::FirstLevelScene>("game_first_level");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::SecondLevelScene>("game_second_level");
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::MainMenu>("main_menu");
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::Option>("option");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::EndScene>("end_scene");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("main_menu");

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(
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
        ES::Plugin::UI::System::ButtonClick,
        ES::Engine::Entity::RemoveTemporaryComponents,
        ES::Plugin::UI::System::UpdateButtonState,
        ES::Plugin::UI::System::UpdateButtonTexture
	);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&](ES::Engine::Core &c) {
		ES::Plugin::Input::Utils::PrintAvailableControllers();

		auto &inputManager = c.GetResource<Input::Resource::InputManager>();
		inputManager.RegisterKeyCallback([](ES::Engine::Core &cbCore, int key, int, int action, int) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(cbCore.GetResource<Window::Resource::Window>().GetGLFWWindow(), true);
			}
		});
	});

	core.RegisterSystem<ES::Engine::Scheduler::Shutdown>(
		[](ES::Engine::Core &c) {
			glfwDestroyWindow(c.GetResource<Window::Resource::Window>().GetGLFWWindow());
			glfwTerminate();
		}
	);

	core.RunCore();

    return 0;
}
