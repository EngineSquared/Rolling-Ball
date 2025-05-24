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

	sp.Use();
	glm::vec3 posOfLight(10.0f, 13.0f, 0.0f);
	float near_plane = 1.0f, far_plane = 50.f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(posOfLight, 
						glm::vec3( 0.0f, 0.0f, 0.0f), 
						glm::vec3( 0.0f, 1.0f,  0.0f));
	
	// make posOfLight rotate around the center
	float radius = 10.0f;
	float nbr_lights = 5.f;
	float scale = 2.f * glm::pi<float>() / nbr_lights;
	lightView = glm::lookAt(glm::vec3(3.f, 10.f, 3.f), 
						glm::vec3( 0.0f, 5.0f, 0.0f), 
						glm::vec3( 0.0f, 1.0f,  0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glUniformMatrix4fv(sp.GetUniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glUniform3fv(sp.GetUniform("CamPos"), 1, glm::value_ptr(core.GetResource<OpenGL::Resource::Camera>().viewer.getViewPoint()));
	sp.Disable();
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

	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1280;
	const unsigned int SHADOW_HEIGHT = 720;
	unsigned int depthMap;

	ES::Plugin::OpenGL::Utils::Texture texture(0, 0, 0, 0);


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
		LoadTextureShadowShader,
		LoadShadowShader,
		[&depthMapFBO, &depthMap, &texture, &SHADOW_WIDTH, &SHADOW_HEIGHT](ES::Engine::Core &core) {
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
				GLsizei length, const GLchar *message, const void *userParam) {
				std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
						  << " Source: " << source
						  << ", Type: " << type
						  << ", ID: " << id
						  << ", Severity: " << severity
						  << ", Message: " << message << std::endl;
			}, 0);
			glfwSetErrorCallback([](int error, const char *description) {
				std::cerr << "GLFW ERROR: " << error << ": " << description << std::endl;
			});
			glGenFramebuffers(1, &depthMapFBO);
			glGenTextures(1, &depthMap);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Add(entt::hashed_string{"depthMap"}, SHADOW_WIDTH, SHADOW_HEIGHT, 1, depthMap);
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
        ES::Plugin::UI::System::UpdateButtonTexture,
		[](ES::Engine::Core &core){
			auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"textureShadow"});
			shaderProgram.Use();
			glUniform3fv(shaderProgram.GetUniform("CamPos"), 1,
						glm::value_ptr(core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().viewer.getViewPoint()));
			shaderProgram.Disable();
		},
		[](ES::Engine::Core &core){
			auto &shaderProgram = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get(entt::hashed_string{"texture"});
			shaderProgram.Use();
			glUniform3fv(shaderProgram.GetUniform("CamPos"), 1,
						glm::value_ptr(core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().viewer.getViewPoint()));
			shaderProgram.Disable();
		}
	);
	bool created = false;
	core.RegisterSystem<ES::Engine::Scheduler::Update>(
		[&depthMapFBO, &depthMap, &created](ES::Engine::Core &core) {
			if (core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().GetCurrentScene() != "game_first_level" || created)
				return;
			created = true;
			core.RegisterSystem<ES::Plugin::RenderingPipeline::ToGPU>(
				[&depthMapFBO, &depthMap](ES::Engine::Core &core) {
					using namespace entt;
					static glm::vec3 posOfLight(10.0f, 13.0f, 0.0f);
					float near_plane = 1.0f, far_plane = 50.f;
					glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, near_plane, far_plane);
					glm::mat4 lightView = glm::lookAt(posOfLight, 
										glm::vec3( 0.0f, 0.0f, 0.0f), 
										glm::vec3( 0.0f, 1.0f,  0.0f));
					
					// make posOfLight rotate around the center
					float radius = 10.0f;
					float nbr_lights = 5.f;
                	float scale = 2.f * glm::pi<float>() / nbr_lights;
					lightView = glm::lookAt(glm::vec3(3.f, 10.f, 3.f), 
										glm::vec3( 0.0f, 5.0f, 0.0f), 
										glm::vec3( 0.0f, 1.0f,  0.0f));

					glm::mat4 lightSpaceMatrix = lightProjection * lightView;

					// use depth shader
					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);

					auto &shad = core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>().Get("shadow"_hs);
					shad.Use();
					glUniformMatrix4fv(shad.GetUniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
					
					auto &view = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().view;
					auto &projection = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().projection;
					core.GetRegistry()
						.view<ES::Plugin::OpenGL::Component::ModelHandle, ES::Plugin::Object::Component::Transform, ES::Plugin::Object::Component::Mesh,
							ES::Plugin::OpenGL::Component::MaterialHandle, ES::Plugin::OpenGL::Component::ShaderHandle>()
						.each([&](auto entity, ES::Plugin::OpenGL::Component::ModelHandle &modelHandle, ES::Plugin::Object::Component::Transform &transform,
								ES::Plugin::Object::Component::Mesh &mesh, ES::Plugin::OpenGL::Component::MaterialHandle &materialHandle,
								ES::Plugin::OpenGL::Component::ShaderHandle &shaderHandle) {
							const auto &glBuffer = core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>().Get(modelHandle.id);
							glm::mat4 modelmat = transform.getTransformationMatrix();
							glm::mat4 mvp = modelmat;
							glUniformMatrix4fv(shad.GetUniform("model"), 1, GL_FALSE, glm::value_ptr(mvp));
							glBuffer.Draw(mesh);
						});
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					auto cameraSize = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size;
					glViewport(0, 0, static_cast<int>(cameraSize.x), static_cast<int>(cameraSize.y));
				}
			);
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
