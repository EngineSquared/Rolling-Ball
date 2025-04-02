#include "PlayerMovement.hpp"

// TODO: the "Body" include should be in ESQ, not here
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "Player.hpp"
#include "InputManager.hpp"
#include "PhysicsManager.hpp"

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

    if (glm::length(force) > 1.0f) {
        force = glm::normalize(force);
    }

    return force;
}

void Game::PlayerMovement(ES::Engine::Core &core)
{
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    glm::vec3 force = GetPlayerMovementForce(core);

    if (glm::length(force) == 0.0f) {
        return;
    }

    // TODO: only move if player is on the floor
    auto ApplyMovementForce = [&](auto &body, const Game::Player &player) {
        if (body == nullptr) {
            return;
        }

        glm::vec3 walkForce(force.x * player.acceleration.x, 0.0f, force.z * player.acceleration.z);

        bodyInterface.AddForce(body->GetID(), JPH::Vec3(walkForce.x, 0.0f, walkForce.z));
        
        auto linearVelocity = body->GetLinearVelocity();
        linearVelocity.SetX(glm::clamp(linearVelocity.GetX(), -player.maxSpeed.x, player.maxSpeed.x));
        linearVelocity.SetZ(glm::clamp(linearVelocity.GetZ(), -player.maxSpeed.z, player.maxSpeed.z));
        bodyInterface.SetLinearVelocity(body->GetID(), linearVelocity);
    };

    core.GetRegistry().view<Game::Player, ES::Plugin::Physics::Component::RigidBody3D>().each([&](auto entity, auto &player, auto &rigidBody) {
        ApplyMovementForce(rigidBody.body, player);
    });

    core.GetRegistry().view<Game::Player, ES::Plugin::Physics::Component::SoftBody3D>().each([&](auto entity, auto &player, auto &softBody) {
        ApplyMovementForce(softBody.body, player);
    });
}