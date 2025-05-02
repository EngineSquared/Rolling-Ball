#include "PlayerMovement.hpp"

#include "Camera.hpp"
#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "Player.hpp"
#include "InputUtils.hpp"
#include "PhysicsManager.hpp"

static glm::vec3 GetPlayerKeyboardMovementForce(ES::Engine::Core &core)
{
    glm::vec3 force(0.0f, 0.0f, 0.0f);

    if (ES::Plugin::Input::Utils::IsKeyPressed(GLFW_KEY_W)) {
        force.z += 1.0f;
    }
    if (ES::Plugin::Input::Utils::IsKeyPressed(GLFW_KEY_S)) {
        force.z -= 1.0f;
    }
    if (ES::Plugin::Input::Utils::IsKeyPressed(GLFW_KEY_A)) {
        force.x -= 1.0f;
    }
    if (ES::Plugin::Input::Utils::IsKeyPressed(GLFW_KEY_D)) {
        force.x += 1.0f;
    }

    if (glm::length(force) > 1.0f) {
        force = glm::normalize(force);
    }

    return force;
}

// Works on PS5 controller, untested on other controllers
static glm::vec3 GetPlayerJoystickMovementForce(ES::Engine::Core &core, int joystickID)
{
    glm::vec3 force(0.0f);

    if (!ES::Plugin::Input::Utils::IsJoystickPresent(joystickID)) {
        return force;
    }

    ES::Plugin::Input::Utils::JoystickAxes axes;
    try {
        axes = ES::Plugin::Input::Utils::GetJoystickAxes(joystickID);
    } catch (const ES::Plugin::Input::InputError &e) {
        return force;
    }

    if (axes.size() >= 2) {
        float x = axes[0];
        float y = axes[1];

        force.x = x;
        force.z = -y;
    }

    if (glm::length(force) > 1.0f) {
        force = glm::normalize(force);
    }

    return force;
}


void Game::PlayerMovement(ES::Engine::Core &core)
{
    auto &camera = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    glm::vec3 keyboardForce = GetPlayerKeyboardMovementForce(core);
    glm::vec3 joystickForce = GetPlayerJoystickMovementForce(core, 0);

    if (glm::length(keyboardForce) == 0.0f && glm::length(joystickForce) == 0.0f) {
        return;
    }

    glm::vec3 force = glm::length(keyboardForce) > 0.0f ? keyboardForce : joystickForce;

    auto viewDir = camera.viewer.getViewDir();
    force = glm::vec3(viewDir.x, 0.0f, viewDir.z) * force.z + glm::vec3(-viewDir.z, 0.0f, viewDir.x) * force.x;

    auto ApplyMovementForce = [&](auto &body, Game::Player &player) {
        if (body == nullptr) {
            return;
        }

        force.x *= player.acceleration.x;
        force.z *= player.acceleration.z;

        bodyInterface.AddForce(body->GetID(), JPH::Vec3(force.x, 0.0f, force.z));
    
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