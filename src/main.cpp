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
#include "LightInfo.hpp"
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
#include "RenderingPipeline.hpp"

using namespace JPH; // NOT RECOMMENDED

using namespace JPH::literals;
using namespace ES::Plugin;

void LoadTextureShadowShader(ES::Engine::Core &core)
{
	// This "using" allow to use "_hs" compile time hashing for strings
	using namespace entt;
	const std::string vertexShader = "asset/shader/textureShadow/textureShadow.vs";
	const std::string fragmentShader = "asset/shader/textureShadow/textureShadow.fs";
	auto &shaderManager = core.GetResource<OpenGL::Resource::ShaderManager>();
    OpenGL::Utils::ShaderProgram &sp = shaderManager.Add("textureShadow"_hs);
    sp.Create();
    sp.InitFromFiles(vertexShader, fragmentShader);
    sp.AddUniform("texture0");
	sp.AddUniform("shadowMap");
	sp.AddUniform("lightSpaceMatrix");
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
}

void LoadShadowShader(ES::Engine::Core &core)
{
	// This "using" allow to use "_hs" compile time hashing for strings
	using namespace entt;
	const std::string vertexShader = "asset/shader/shadow/shadow.vs";
	const std::string fragmentShader = "asset/shader/shadow/shadow.fs";
	auto &shaderManager = core.GetResource<OpenGL::Resource::ShaderManager>();
    OpenGL::Utils::ShaderProgram &sp = shaderManager.Add("shadow"_hs);
    sp.Create();
    sp.InitFromFiles(vertexShader, fragmentShader);
   	sp.AddUniform("lightSpaceMatrix");
	sp.AddUniform("model");
}

class DirectionalLight
{
public:
	bool enabled = true;
	GLuint depthMapFBO = 0;
	GLuint depthMap = 0;
	GLsizei SHADOW_WIDTH = 1280;
	GLsizei SHADOW_HEIGHT = 720;

	glm::vec3 posOfLight = glm::vec3(6.f, 20.f, 6.f);
	float near_plane = 1.0f;
	float far_plane = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, 1.0f, 50.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(6.f, 20.f, 6.f), 
									  glm::vec3(0.0f, 5.0f, 0.0f), 
									  glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
};

void GenerateDirectionalLightFramebuffer(ES::Engine::Core &core)
{
	auto &light = core.GetResource<DirectionalLight>();

	glGenFramebuffers(1, &light.depthMapFBO);
}

void GenerateDirectionalLightTexture(ES::Engine::Core &core)
{
	auto &light = core.GetResource<DirectionalLight>();

	// Create a texture for the shadow map
	glGenTextures(1, &light.depthMap);

	// Setup the texture
	glBindTexture(GL_TEXTURE_2D, light.depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, light.SHADOW_WIDTH, light.SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Register the texture in the TextureManager, this is only for debug purposes
	core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Add(entt::hashed_string{"depthMap"}, light.SHADOW_WIDTH, light.SHADOW_HEIGHT, 1, light.depthMap);
}

void BindDirectionalLightTextureToFramebuffer(ES::Engine::Core &core)
{
	auto &light = core.GetResource<DirectionalLight>();

	// Attach the texture to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, light.depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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

	core.RegisterResource<DirectionalLight>(DirectionalLight{
		.enabled = true
	});

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(
		[](const ES::Engine::Core &c) {
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
				GLsizei length, const GLchar *message, const void *userParam) {
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
		LoadTextureShadowShader,
		LoadShadowShader,
		GenerateDirectionalLightFramebuffer,
		GenerateDirectionalLightTexture,
		BindDirectionalLightTextureToFramebuffer,
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
        ES::Plugin::UI::System::UpdateButtonTexture,
		[](ES::Engine::Core &core){
			const auto &light = core.GetResource<DirectionalLight>();
			if (!light.enabled)
				return;
			auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"textureShadow"});
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
		},
		[](ES::Engine::Core &core){
			const auto &light = core.GetResource<DirectionalLight>();
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
		},
		[](ES::Engine::Core &core){
			const auto &light = core.GetResource<DirectionalLight>();
			if (!light.enabled)
				return;
			auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"shadow"});
			
			shaderProgram.Use();

			// Link Light Space Matrix to the shader
			glUniformMatrix4fv(shaderProgram.GetUniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
			
			shaderProgram.Disable();
		}
	);

	core.RegisterSystem<ES::Plugin::RenderingPipeline::ToGPU>(
		// Render the shadow map
		[](ES::Engine::Core &core) {
			const auto &light = core.GetResource<DirectionalLight>();
			if (!light.enabled)
				return;
			// Setup the framebuffer for shadow mapping
			glViewport(0, 0, light.SHADOW_WIDTH, light.SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, light.depthMapFBO);
			glCullFace(GL_FRONT);
			glClear(GL_DEPTH_BUFFER_BIT);

			// Render mesh on the shadow map
			auto &shad = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"shadow"});
			shad.Use();
			core.GetRegistry()
				.view<ES::Plugin::OpenGL::Component::ModelHandle, ES::Plugin::Object::Component::Transform, ES::Plugin::Object::Component::Mesh>()
				.each([&](auto entity, ES::Plugin::OpenGL::Component::ModelHandle &modelHandle, ES::Plugin::Object::Component::Transform &transform,
						ES::Plugin::Object::Component::Mesh &mesh) {
					const auto &glBuffer = core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>().Get(modelHandle.id);
					glm::mat4 modelmat = transform.getTransformationMatrix();
					glUniformMatrix4fv(shad.GetUniform("model"), 1, GL_FALSE, glm::value_ptr(modelmat));
					glBuffer.Draw(mesh);
				}
			);
			shad.Disable();
				
			// Put the default state back
			glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			auto cameraSize = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size;
			glViewport(0, 0, static_cast<int>(cameraSize.x), static_cast<int>(cameraSize.y));
		}
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
