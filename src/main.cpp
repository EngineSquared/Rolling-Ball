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

#include "SpawnPlayer.hpp"
#include "PointCameraToPlayer.hpp"
#include "PlayerMovement.hpp"
#include "PlayerJump.hpp"

// #include "Terrain.hpp"
#include "Generator.hpp"

#include "GameScene.hpp"
#include "MainMenu.hpp"

#include <iostream>

// Jolt includes
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

static void UpdateButtonTextureColor(ES::Plugin::UI::Component::Button &button,
	ES::Plugin::OpenGL::Component::Sprite &sprite)
{
	auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType);
	switch (button.state)
	{
		using enum ES::Plugin::UI::Component::Button::State;
		case Normal: sprite.color = displayType.normalColor; break;
		case Hover: sprite.color = displayType.hoverColor; break;
		case Pressed: sprite.color = displayType.pressedColor; break;
	}
}

static void UpdateButtonTextureImage(
    [[maybe_unused]] ES::Plugin::UI::Component::Button &button,
	[[maybe_unused]] ES::Plugin::OpenGL::Component::Sprite &sprite)
{
	// TODO: Implement texture usage for image button
	// auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType);
	// switch (button.state)
	// {
	// 	using enum ES::Plugin::UI::Component::Button::State;
	// 	// case Normal: sprite.textureID = displayType.normalImageID; break;
	// 	// case Hover: sprite.textureID = displayType.hoverImageID; break;
	// 	// case Pressed: sprite.textureID = displayType.pressedImageID; break;
	// }
}

static void UpdateButtonTexture(ES::Engine::Core &core)
{
	auto view = core.GetRegistry()
		.view<ES::Plugin::UI::Component::Button, ES::Plugin::OpenGL::Component::Sprite,
	ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
	for (auto entity : view)
	{
		auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
		auto &sprite = view.get<ES::Plugin::OpenGL::Component::Sprite>(entity);
		if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType))
		{
			UpdateButtonTextureColor(button, sprite);
		}
		else if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType))
		{
			UpdateButtonTextureImage(button, sprite);
		}
	}
}

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

int main(void)
{
    ES::Engine::Core core;

	core.AddPlugins<OpenGL::Plugin, Physics::Plugin>();

	core.RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());

	core.RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());
	
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::FirstLevelScene>("game_first_level");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::SecondLevelScene>("game_second_level");
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<Game::MainMenu>("main_menu");
	core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("main_menu");

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(
		[](ES::Engine::Core &c) {
			c.GetResource<OpenGL::Resource::Camera>().viewer.lookFrom(glm::vec3(0.0f, 10.0f, -20.0f));
		},
		[](ES::Engine::Core &c) {
			c.GetResource<Physics::Resource::PhysicsManager>().GetPhysicsSystem().OptimizeBroadPhase();
			c.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(2);
			c.GetScheduler<ES::Engine::Scheduler::FixedTimeUpdate>().SetTickRate(1.0f / 240.0f);
		},
		LoadNormalShader
	);

	core.RegisterSystem<ES::Engine::Scheduler::Update>(
		ES::Plugin::Scene::System::UpdateScene,
        ES::Plugin::UI::System::ButtonClick,
        ES::Engine::Entity::RemoveTemporaryComponents,
        ES::Plugin::UI::System::UpdateButtonState,
        UpdateButtonTexture
	);

	core.RunCore();

    glfwDestroyWindow(core.GetResource<Window::Resource::Window>().GetGLFWWindow());
    glfwTerminate();

    return 0;
}