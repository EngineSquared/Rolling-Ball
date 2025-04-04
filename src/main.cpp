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

#include "Terrain.hpp"

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

ES::Engine::Entity CreateFloor(ES::Engine::Core &core)
{
	glm::vec3 floor_position = glm::vec3(0.0f, -8.0f, 0.0f);
	glm::vec3 floor_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	// Rotated slightly in a 12° angle, so that the sphere will roll on it
	glm::quat floor_rotation = glm::rotate(glm::quat(0.0f, 0.0f, 0.0f, 1.0f), glm::radians(12.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	Vec3 floor_size = Vec3(10.0f, 1.0f, 10.0f);

	std::shared_ptr<BoxShapeSettings> floor_shape_settings = std::make_shared<BoxShapeSettings>(floor_size);
	ES::Engine::Entity floor = core.CreateEntity();

	floor.AddComponent<Object::Component::Transform>(core, floor_position, floor_scale, floor_rotation);
	floor.AddComponent<Physics::Component::RigidBody3D>(core, floor_shape_settings, EMotionType::Static, Physics::Utils::Layers::NON_MOVING);

	// Add a mesh to it for rendering
	floor.AddComponent<OpenGL::Component::ShaderHandle>(core, "default");
    floor.AddComponent<OpenGL::Component::MaterialHandle>(core, "default");
    floor.AddComponent<OpenGL::Component::ModelHandle>(core, "floor");

    Object::Component::Mesh mesh;
	{
		// Generate a box
		const float width = floor_size.GetX();
		const float height = floor_size.GetY();
		const float depth = floor_size.GetZ();

		// Generate vertices and normals
		const glm::vec3 front_bottom_left = glm::vec3(-width, -height, -depth);
		const glm::vec3 front_bottom_right = glm::vec3(width, -height, -depth);
		const glm::vec3 front_top_left = glm::vec3(-width, height, -depth);
		const glm::vec3 front_top_right = glm::vec3(width, height, -depth);
		const glm::vec3 back_bottom_left = glm::vec3(-width, -height, depth);
		const glm::vec3 back_bottom_right = glm::vec3(width, -height, depth);
		const glm::vec3 back_top_left = glm::vec3(-width, height, depth);
		const glm::vec3 back_top_right = glm::vec3(width, height, depth);

		// Front face
		mesh.vertices.push_back(front_bottom_left);
		mesh.vertices.push_back(front_bottom_right);
		mesh.vertices.push_back(front_top_left);
		mesh.vertices.push_back(front_top_right);

		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));

		// Back face
		mesh.vertices.push_back(back_bottom_left);
		mesh.vertices.push_back(back_bottom_right);
		mesh.vertices.push_back(back_top_left);
		mesh.vertices.push_back(back_top_right);

		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

		// Top face
		mesh.vertices.push_back(front_top_left);
		mesh.vertices.push_back(front_top_right);
		mesh.vertices.push_back(back_top_left);
		mesh.vertices.push_back(back_top_right);

		mesh.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

		// Bottom face
		mesh.vertices.push_back(front_bottom_left);
		mesh.vertices.push_back(front_bottom_right);
		mesh.vertices.push_back(back_bottom_left);
		mesh.vertices.push_back(back_bottom_right);

		mesh.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

		// Left face
		mesh.vertices.push_back(front_bottom_left);
		mesh.vertices.push_back(front_top_left);
		mesh.vertices.push_back(back_bottom_left);
		mesh.vertices.push_back(back_top_left);

		mesh.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

		// Right face
		mesh.vertices.push_back(front_bottom_right);
		mesh.vertices.push_back(front_top_right);
		mesh.vertices.push_back(back_bottom_right);
		mesh.vertices.push_back(back_top_right);

		mesh.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		mesh.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

		// Generate indices for triangle strips

		mesh.indices = {
			// Front
			2, 1, 0,
			2, 3, 1,
			// Back
			4, 5, 6,
			5, 7, 6,
			// Bottom
			10, 9, 8,
			10, 11, 9,
			// Top
			12, 13, 14,
			13, 15, 14,
			// Left
			18, 17, 16,
			18, 19, 17,
			// Right
			20, 21, 22,
			21, 23, 22,
		};
	}
	floor.AddComponent<Object::Component::Mesh>(core, mesh);
	floor.AddComponent<Game::Terrain>(core);


	return floor;
}
struct Name {
	std::string value;
};

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

	core.RegisterResource<ES::Plugin::Input::Resource::InputManager>(std::move(ES::Plugin::Input::Resource::InputManager()));

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&](ES::Engine::Core &core) {
		core.GetResource<OpenGL::Resource::Camera>().viewer.lookFrom(glm::vec3(0.0f, 10.0f, -20.0f));
	});

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&](ES::Engine::Core &core) {
		auto floor = CreateFloor(core);
	});


	core.RegisterSystem<ES::Engine::Scheduler::Startup>(Game::SpawnPlayer);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&](ES::Engine::Core &core) {
		core.GetResource<Physics::Resource::PhysicsManager>().GetPhysicsSystem().OptimizeBroadPhase();
		core.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(2);

		core.GetScheduler<ES::Engine::Scheduler::FixedTimeUpdate>().SetTickRate(1.0f / 240.0f);
	});

	// WARNING: adding systems inside a system will not work properly because it will not be call in the current frame / run of schedulers
	core.RegisterSystem<ES::Engine::Scheduler::Startup>(LoadNormalShader);
	core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
		Game::PointCameraToPlayer, Game::PlayerMovement
	);
	core.RegisterSystem<ES::Engine::Scheduler::Update>(Game::PlayerJump);


	core.RunCore();

    glfwDestroyWindow(core.GetResource<Window::Resource::Window>().GetGLFWWindow());
    glfwTerminate();

    return 0;
}