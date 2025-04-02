#include "PointCameraToPlayer.hpp"

#include "Camera.hpp"
#include "Transform.hpp"
#include "PlayerTag.hpp"

void Game::PointCameraToPlayer(ES::Engine::Core &core)
{
    auto &camera = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>();

    core.GetRegistry().view<Game::PlayerTag, ES::Plugin::Object::Component::Transform>().each([&](auto entity, auto &transform) {
        printf("Position: %f %f %f\n", transform.position.x, transform.position.y, transform.position.z);
        camera.viewer.centerAt(transform.position);
    });
}
