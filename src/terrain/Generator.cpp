#include "Transform.hpp"

#include "JoltPhysics.hpp"
#include "OpenGL.hpp"
#include "OBJLoader.hpp"

#include "Generator.hpp"

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

Game::TerrainType Game::GetRandomTerrainType(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> dist(0, 5);
    return static_cast<Game::TerrainType>(dist(rng));
}

void Game::GenerateAndInstantiateTerrain(ES::Engine::Core &core) {
	Game::Terrain terrain;

    GenerateTerrain(terrain);
    for (const auto &piece : terrain.pieces) {
        ES::Engine::Entity entity = Game::CreateTerrainPiece(core, piece);
    }
}

void Game::GenerateTerrain(Game::Terrain &terrain)
{
    for (int i = 0; i < terrain.segmentCount; ++i)
    {
        auto type = GetRandomTerrainType(terrain.rng);

        Game::TerrainPiece piece;
        piece.type = type;
        piece.position = terrain.segmentsPositionOffset + terrain.segmentsGapPositionOffset * glm::vec3(static_cast<float>(i));
        piece.scale = terrain.segmentsScale;
        piece.rotationAngle = terrain.segmentsRotation;
        terrain.pieces.push_back(piece);
    }
}

ES::Engine::Entity Game::CreateTerrainPiece(ES::Engine::Core &core, const TerrainPiece &piece)
{
    ES::Engine::Entity terrainEntity = core.CreateEntity();

    terrainEntity.AddComponent<Object::Component::Transform>(core, piece.position, piece.scale, piece.rotationAngle);

    terrainEntity.AddComponent<OpenGL::Component::ShaderHandle>(core, "default");
    terrainEntity.AddComponent<OpenGL::Component::MaterialHandle>(core, "default");

    std::string modelName;
    switch (piece.type)
    {
    case TerrainType::Flat:
        modelName = "floor";
        break;
    case TerrainType::Ramp:
        modelName = "ramp";
        break;
    case TerrainType::CurveLeft:
        modelName = "curve_left";
        break;
    case TerrainType::CurveRight:
        modelName = "curve_right";
        break;
    case TerrainType::Wave:
        modelName = "wave";
        break;
    default:
        modelName = "default";
        break;
    }
    terrainEntity.AddComponent<OpenGL::Component::ModelHandle>(core, modelName);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<std::seed_seq::result_type> indices;

    std::string modelPath;
    if (piece.type == TerrainType::Flat) modelPath = "asset/models/straight.obj";
    else if (piece.type == TerrainType::Ramp) modelPath = "asset/models/ramp.obj";
    else if (piece.type == TerrainType::CurveLeft) modelPath = "asset/models/curve_left.obj";
    else if (piece.type == TerrainType::CurveRight) modelPath = "asset/models/curve_right.obj";
    else if (piece.type == TerrainType::Wave) modelPath = "asset/models/wave.obj";
    else modelPath = "asset/models/straight.obj";

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
        ES::Utils::Log::Info(fmt::format("Loaded terrain 3D model for {} successfully", modelPath));
    } else {
        ES::Utils::Log::Error(fmt::format("Failed to load terrain 3D model for {}", modelPath));
    }

    terrainEntity.AddComponent<Game::Terrain>(core);

    return terrainEntity;
}
