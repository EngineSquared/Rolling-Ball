#include "PointCameraToPlayer.hpp"

#include "resource/Camera.hpp"
#include "component/Transform.hpp"
#include "Player.hpp"

void Game::PointCameraToPlayer(ES::Engine::Core &core)
{
    auto &camera = core.GetResource<ES::Plugin::OpenGL::Resource::Camera>();

    core.GetRegistry().view<Game::Player, ES::Plugin::Object::Component::Transform>().each([&](auto, auto &transform) {
        camera.viewer.centerAt(transform.position);
    });
}
