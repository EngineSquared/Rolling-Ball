#include "Core.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "FixedTimeUpdate.hpp"
#include "InputManager.hpp"

#include "player/SpawnPlayer.hpp"
#include "player/PointCameraToPlayer.hpp"
#include "player/PlayerMovement.hpp"

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

	floor.AddComponent<ES::Plugin::Object::Component::Transform>(core, floor_position, floor_scale, floor_rotation);
	floor.AddComponent<ES::Plugin::Physics::Component::RigidBody3D>(core, floor_shape_settings, EMotionType::Static, Physics::Utils::Layers::NON_MOVING);

	// Add a mesh to it for rendering
	floor.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "default");
    floor.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, "default");
    floor.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, "floor");

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

	floor.AddComponent<ES::Plugin::Object::Component::Mesh>(core, mesh);

	return floor;
}

int main(void)
{
    ES::Engine::Core core;

	core.AddPlugins<ES::Plugin::OpenGL::Plugin, ES::Plugin::Physics::Plugin>();

	core.RegisterResource<ES::Plugin::Input::Resource::InputManager>(std::move(ES::Plugin::Input::Resource::InputManager()));

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&core](ES::Engine::Core &core) {
		auto floor = CreateFloor(core);
	});

	core.RegisterSystem<ES::Engine::Scheduler::Startup>(Game::SpawnPlayer);

	core.RegisterSystem<ES::Engine::Scheduler::Startup>([&core](ES::Engine::Core &core) {
		core.GetResource<Physics::Resource::PhysicsManager>().GetPhysicsSystem().OptimizeBroadPhase();
		core.GetResource<Physics::Resource::PhysicsManager>().SetCollisionSteps(2);

		core.GetScheduler<ES::Engine::Scheduler::FixedTimeUpdate>().SetTickRate(1.0f / 240.0f);
	});

	core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
		Game::PointCameraToPlayer, Game::PlayerMovement
	);

	core.RunCore();

    glfwDestroyWindow(core.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow());
    glfwTerminate();

    return 0;
}