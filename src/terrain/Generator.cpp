#include "Transform.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"
#include "OBJLoader.hpp"

#include "Generator.hpp"
#include "FinishSegment.hpp"

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

using namespace ES::Plugin;

static const std::string GetModelPathFromTerrainType(const Game::TerrainType type) {
    static const std::unordered_map<Game::TerrainType, std::string> modelPaths = {
        {Game::TerrainType::Flat, "asset/models/straight.obj"},
        {Game::TerrainType::Ramp, "asset/models/ramp.obj"},
        {Game::TerrainType::CurveLeft, "asset/models/curve_left.obj"},
        {Game::TerrainType::CurveRight, "asset/models/curve_right.obj"},
        {Game::TerrainType::Wave, "asset/models/wave.obj"},
        {Game::TerrainType::Jump, "asset/models/jump.obj"},
        {Game::TerrainType::JumpSingle, "asset/models/jump_single.obj"},
        {Game::TerrainType::ObstacleA, "asset/models/obstacle1.obj"},
        {Game::TerrainType::ObstacleB, "asset/models/obstacle2.obj"},
        {Game::TerrainType::ObstacleC, "asset/models/obstacle3.obj"},
        {Game::TerrainType::MovingObstacleA, "asset/models/moving_obstacle1.obj"},
        {Game::TerrainType::MovingObstacleB, "asset/models/moving_obstacle2.obj"},
        {Game::TerrainType::Finish, "asset/models/finish.obj"},
    };

    auto it = modelPaths.find(type);
    if (it != modelPaths.end()) {
        return it->second;
    }
    return "asset/models/straight.obj"; // Default fallback
}

Game::SegmentMetrics Game::GetSegmentMetrics(Game::TerrainType type, const glm::vec3 &scale, const glm::quat &rotation)
{
    if (cachedMetrics.contains(type))
        return cachedMetrics[type];

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<std::seed_seq::result_type> indices;
    std::string modelPath = GetModelPathFromTerrainType(type);

    if (!ES::Plugin::Object::Resource::OBJLoader::loadModel(modelPath, vertices, normals, texCoords, indices)) {
        ES::Utils::Log::Error(fmt::format("Failed to load model for metrics: {}", modelPath));
        return {20.0f, 0.0f}; // fallback
    }

    glm::mat4 transform = glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
    glm::vec3 minPos(FLT_MAX);
    glm::vec3 maxPos(-FLT_MAX);

    for (const auto &v : vertices) {
        glm::vec3 transformed = glm::vec3(transform * glm::vec4(v, 1.0f));
        minPos = glm::min(minPos, transformed);
        maxPos = glm::max(maxPos, transformed);
    }
    glm::vec3 delta = maxPos - minPos;

    float length = std::floor(std::max({delta.x, delta.y, delta.z}));

    // Hardcoding the length of segments because it needs a gap
    if (type == Game::TerrainType::JumpSingle)
        length = 40.0f;
    else if (type == Game::TerrainType::MovingObstacleA)
        length = 15.0f;

    SegmentMetrics metrics;
    metrics.lengthZ = length;
    metrics.heightDeltaY = 0.0f;
    cachedMetrics[type] = metrics;
    return metrics;
}

Game::TerrainType Game::GetRandomTerrainType(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> dist(0, static_cast<int>(Game::TerrainType::Finish) - 1);
    return static_cast<Game::TerrainType>(dist(rng));
}

std::vector<ES::Engine::Entity> Game::InstantiateLoadedTerrain(ES::Engine::Core &core, const Game::Terrain &terrain)
{
    std::vector<ES::Engine::Entity> terrainEntities;

    ES::Engine::Entity terrainRootEntity = core.CreateEntity();
    for (const auto &piece : terrain.pieces) {
        terrainEntities.push_back(Game::CreateTerrainPiece(core, piece));
    }
    terrainRootEntity.AddComponent<Game::Terrain>(core, terrain);
    return terrainEntities;
}

std::vector<ES::Engine::Entity> Game::GenerateAndInstantiateTerrain(ES::Engine::Core &core) {
	Game::Terrain terrain;
    std::vector<ES::Engine::Entity> terrainEntities;

    GenerateTerrain(terrain);
    ES::Engine::Entity terrainRootEntity = core.CreateEntity();
    for (const auto &piece : terrain.pieces) {
        terrainEntities.push_back(Game::CreateTerrainPiece(core, piece));
    }
    terrainRootEntity.AddComponent<Game::Terrain>(core, terrain);
    return terrainEntities;
}

void Game::GenerateTerrain(Game::Terrain &terrain)
{
    glm::vec3 currentPosition = terrain.segmentsPositionOffset;

    for (int i = 0; i <= terrain.segmentCount; ++i)
    {
        auto type = GetRandomTerrainType(terrain.rng);
        if (i == 0)
            type = Game::TerrainType::Flat;
        if (i == terrain.segmentCount)
            type = Game::TerrainType::Finish;

        SegmentMetrics metrics = GetSegmentMetrics(type, terrain.segmentsScale, terrain.segmentsRotation);
        TerrainPiece piece;
        piece.type = type;
        piece.scale = terrain.segmentsScale;
        piece.rotationAngle = terrain.segmentsRotation;
        piece.position = currentPosition;
        glm::vec3 forward = glm::rotate(terrain.segmentsRotation, glm::vec3(-1, 0, 0));
        glm::vec3 up = glm::rotate(terrain.segmentsRotation, glm::vec3(0, -1, 0));
        currentPosition += forward * metrics.lengthZ;
        currentPosition += up * metrics.heightDeltaY;
        terrain.pieces.push_back(piece);
    }
}

ES::Engine::Entity Game::CreateTerrainPiece(ES::Engine::Core &core, const TerrainPiece &piece)
{
    ES::Engine::Entity terrainEntity = core.CreateEntity();

    terrainEntity.AddComponent<Object::Component::Transform>(core, piece.position, piece.scale, piece.rotationAngle);
    terrainEntity.AddComponent<OpenGL::Component::ShaderHandle>(core, "textureShadow");
    terrainEntity.AddComponent<OpenGL::Component::MaterialHandle>(core, "default");
    if (piece.type == TerrainType::Finish)
        terrainEntity.AddComponent<Game::Finish>(core);

    std::string modelName = GetModelPathFromTerrainType(piece.type);
    terrainEntity.AddComponent<OpenGL::Component::ModelHandle>(core, modelName);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<std::seed_seq::result_type> indices;
    std::string modelPath = GetModelPathFromTerrainType(piece.type);

    if (ES::Plugin::Object::Resource::OBJLoader::loadModel(modelPath, vertices, normals, texCoords, indices)) {
        Object::Component::Mesh mesh;
        mesh.vertices = vertices;
        mesh.normals = normals;
        mesh.indices = indices;

        terrainEntity.AddComponent<Object::Component::Mesh>(core, mesh);
        JPH::VertexList vertexList;
        vertexList.reserve(vertices.size());
        for (const auto& v : vertices) {
            vertexList.emplace_back(v.x * piece.scale.x, v.y * piece.scale.y, v.z * piece.scale.z);
        }

        JPH::IndexedTriangleList triangleList;
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            triangleList.emplace_back(
                static_cast<uint32_t>(indices[i]),
                static_cast<uint32_t>(indices[i + 1]),
                static_cast<uint32_t>(indices[i + 2])
            );
        }

        auto shapeSettings = std::make_shared<JPH::MeshShapeSettings>(vertexList, triangleList);
        terrainEntity.AddComponent<Physics::Component::RigidBody3D>(
            core, shapeSettings, JPH::EMotionType::Static, Physics::Utils::Layers::NON_MOVING
        );
        terrainEntity.AddComponent<Game::TerrainPiece>(core, piece);
        ES::Utils::Log::Info(fmt::format("Loaded terrain 3D model for {} successfully", modelPath));
    } else {
        ES::Utils::Log::Error(fmt::format("Failed to load terrain 3D model for {}", modelPath));
    }
    return terrainEntity;
}
