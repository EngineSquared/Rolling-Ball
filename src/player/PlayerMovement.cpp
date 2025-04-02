#include "PlayerMovement.hpp"

// TODO: the "Body" include should be in ESQ, not here
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "RigidBody3D.hpp"
#include "PlayerTag.hpp"
#include "InputManager.hpp"

static glm::vec3 GetPlayerMovementForce(ES::Engine::Core &core)
{
    glm::vec3 force(0.0f, 0.0f, 0.0f);
    auto &input = core.GetResource<ES::Plugin::Input::Resource::InputManager>();

    if (input.IsKeyPressed(GLFW_KEY_Z)) {
        force.z += 1.0f;
    }
    if (input.IsKeyPressed(GLFW_KEY_S)) {
        force.z -= 1.0f;
    }
    if (input.IsKeyPressed(GLFW_KEY_Q)) {
        force.x -= 1.0f;
    }
    if (input.IsKeyPressed(GLFW_KEY_D)) {
        force.x += 1.0f;
    }
    printf("Force: %f %f %f\n", force.x, force.y, force.z);
    return glm::normalize(force);
}

void Game::PlayerMovement(ES::Engine::Core &core)
{
    glm::vec3 force = GetPlayerMovementForce(core);

    // TODO: only move if player is on the floor
    // TODO: do it for soft bodies as well
    core.GetRegistry().view<Game::PlayerTag, ES::Plugin::Physics::Component::RigidBody3D>().each([&](auto entity, auto &rigidBody) {
        if (rigidBody.body == nullptr) {
            return;
        }

        // Maybe add torque instead?
        rigidBody.body->AddForce(JPH::Vec3(force.x, force.y, force.z));
    });
}