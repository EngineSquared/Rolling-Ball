#include "SpawnPlayer.hpp"

#include "Engine.hpp"
#include "Entity.hpp"
#include "Transform.hpp"
#include "Player.hpp"
#include "Camera.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"

#include <iostream>

// Jolt includes
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

using namespace JPH::literals;
using namespace ES::Plugin;

static ES::Engine::Entity CreateSphere(ES::Engine::Core &core, bool isSoftBody, int subdivisionLevel, const glm::vec3 &initialPosition)
{
    constexpr float radius = 0.5f;

    struct Triangle { int v0, v1, v2; };
    std::vector<glm::vec3> vertices;
    std::vector<Triangle> triangles;

    const float t = (1.0f + sqrt(5.0f)) / 2.0f;
    vertices = {
        {-1,  t,  0}, {1,  t,  0}, {-1, -t,  0}, {1, -t,  0},
        {0, -1,  t}, {0,  1,  t}, {0, -1, -t}, {0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };

    for (auto& v : vertices) v = glm::normalize(v) * radius;

    triangles = {
        {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
        {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
        {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
        {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };

    std::unordered_map<uint64_t, int> midpoints;
    auto midpoint = [&](int v1, int v2) -> int {
        uint64_t key = (uint64_t)std::min(v1, v2) << 32 | std::max(v1, v2);
        if (midpoints.count(key)) return midpoints[key];
        glm::vec3 mid = glm::normalize(vertices[v1] + vertices[v2]) * radius;
        int index = vertices.size();
        vertices.push_back(mid);
        midpoints[key] = index;
        return index;
    };

    for (int i = 0; i < subdivisionLevel; ++i) {
        std::vector<Triangle> newTriangles;
        for (auto& tri : triangles) {
            int a = midpoint(tri.v0, tri.v1);
            int b = midpoint(tri.v1, tri.v2);
            int c = midpoint(tri.v2, tri.v0);
            newTriangles.push_back({tri.v0, a, c});
            newTriangles.push_back({tri.v1, b, a});
            newTriangles.push_back({tri.v2, c, b});
            newTriangles.push_back({a, b, c});
        }
        triangles = newTriangles;
    }

    Object::Component::Mesh mesh;
    for (auto& v : vertices) {
        mesh.vertices.push_back(v);
        mesh.normals.push_back(glm::normalize(v));
    }
    for (auto& tri : triangles) {
        mesh.indices.push_back(tri.v0);
        mesh.indices.push_back(tri.v1);
        mesh.indices.push_back(tri.v2);
    }

    auto &textureManager = core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
    auto &texture = textureManager.Add(entt::hashed_string{"default"}, "asset/textures/default.png");

    mesh.texCoords.resize(mesh.vertices.size());
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        const auto& vertex = mesh.vertices[i];
        float u = 0.5f + atan2(vertex.z, vertex.x) / (2.0f * M_PI);
        float v = 0.5f - asin(vertex.y / radius) / M_PI;
        mesh.texCoords[i] = glm::vec2(u, v);
    }

    ES::Engine::Entity sphere = core.CreateEntity();

    sphere.AddComponent<ES::Plugin::Object::Component::Transform>(core, initialPosition, glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    sphere.AddComponent<ES::Plugin::Object::Component::Mesh>(core, mesh);

    if (isSoftBody) {
        sphere.AddComponent<ES::Plugin::Physics::Component::SoftBody3D>(core);
    } else {
        auto sphere_shape_settings = std::make_shared<JPH::SphereShapeSettings>(radius);
        sphere.AddComponent<ES::Plugin::Physics::Component::RigidBody3D>(core, sphere_shape_settings, JPH::EMotionType::Dynamic, Physics::Utils::Layers::MOVING);
    }

    return sphere;
}

ES::Engine::Entity Game::SpawnPlayer(ES::Engine::Core &core)
{
    auto initialPosition = glm::vec3(0.0f, 10.0f, 0.0f);

    auto player = CreateSphere(core, false, 2, initialPosition);

    player.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "texture");
    player.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, "default");
    player.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, "player");
    player.AddComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core, "default");
    player.AddComponent<Game::Player>(core);
    auto &camera = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>();
    camera.viewer.centerAt(initialPosition);
    camera.viewer.lookFrom(glm::vec3(0.0f, 20.0f, -20.0f));

    return player;
}

void Game::RespawnPlayer(ES::Engine::Core &core)
{
    glm::vec3 initialPosition = glm::vec3(0.0f, 10.0f, 0.0f);
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    auto CancelMovementForce = [&](auto &body, Game::Player &player) {
        if (body == nullptr) {
            return;
        }

        bodyInterface.SetLinearVelocity(body->GetID(), JPH::Vec3::sZero());
        bodyInterface.SetAngularVelocity(body->GetID(), JPH::Vec3::sZero());
    };

    core.GetRegistry()
        .view<Game::Player, ES::Plugin::Object::Component::Transform, ES::Plugin::Physics::Component::RigidBody3D>()
        .each([&](auto entity, auto &player, auto &transform, auto &rigidbody) {
            if (transform.position.y < -20.0f) {
                CancelMovementForce(rigidbody.body, player);
                transform.position = initialPosition;
            }
    });
}
