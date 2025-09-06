#include "Events.hpp"
#include "Terrain.hpp"
#include "component/RigidBody3D.hpp"
#include "component/Transform.hpp"
#include "scheduler/RelativeTimeUpdate.hpp"

void Game::MoveSegmentsSideway(ES::Engine::Core &core)
{
    static float time = 0.0f;
    time += core.GetScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();

    float amplitude = 3.5f;
    float frequency = 2.0f;

    core.GetRegistry()
        .view<Game::TerrainPiece, ES::Plugin::Physics::Component::RigidBody3D, ES::Plugin::Object::Component::Transform>()
        .each([&](Game::TerrainPiece &entity, auto &, auto &transform) {
            if (entity.type == Game::TerrainType::MovingObstacleA) {
                float baseX = entity.position.x;
                float xOffset = amplitude * std::sin(frequency * time);

                transform.position.x = baseX + xOffset;
            }
        }
    );
}

void Game::MoveSegmentsSquish(ES::Engine::Core &core)
{
    static float time = 0.0f;
    time += core.GetScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();

    float minScale = 0.1f;
    float maxScale = 2.0f;
    float frequency = 2.0f;

    core.GetRegistry()
        .view<Game::TerrainPiece, ES::Plugin::Physics::Component::RigidBody3D, ES::Plugin::Object::Component::Transform>()
        .each([&](Game::TerrainPiece &entity, auto &, auto &transform) {
            if (entity.type == Game::TerrainType::MovingObstacleB) {
                float normalizedSin = (std::sin(frequency * time) + 1.0f) / 2.0f;
                float scaledZ = minScale + (maxScale - minScale) * normalizedSin;

                transform.scale.z = scaledZ;
            }
        });
}