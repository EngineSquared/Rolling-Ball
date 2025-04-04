#include "Scene.hpp"

#include "PlayerJump.hpp"
#include "PlayerMovement.hpp"
#include "PointCameraToPlayer.hpp"
#include "SpawnPlayer.hpp"
#include "Core.hpp"
#include "UI.hpp"
#include "HasChanged.hpp"
#include <variant>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

using namespace ES::Plugin;

static void UpdateButtonTextureColor(ES::Plugin::UI::Component::Button &button,
	ES::Plugin::OpenGL::Component::Sprite &sprite)
{
	auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType);
	switch (button.state)
	{
		using enum ES::Plugin::UI::Component::Button::State;
		case Normal: sprite.color = displayType.normalColor; break;
		case Hover: sprite.color = displayType.hoverColor; break;
		case Pressed: sprite.color = displayType.pressedColor; break;
	}
}

static void UpdateButtonTextureImage(ES::Plugin::UI::Component::Button &button,
	ES::Plugin::OpenGL::Component::Sprite &sprite)
{
	// TODO: Implement texture usage for image button
	// auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType);
	// switch (button.state)
	// {
	// 	using enum ES::Plugin::UI::Component::Button::State;
	// 	// case Normal: sprite.textureID = displayType.normalImageID; break;
	// 	// case Hover: sprite.textureID = displayType.hoverImageID; break;
	// 	// case Pressed: sprite.textureID = displayType.pressedImageID; break;
	// }
}

static void UpdateButtonTexture(ES::Engine::Core &core)
{
	auto view = core.GetRegistry()
		.view<ES::Plugin::UI::Component::Button, ES::Plugin::OpenGL::Component::Sprite,
	ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
	for (auto entity : view)
	{
		auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
		auto &sprite = view.get<ES::Plugin::OpenGL::Component::Sprite>(entity);
		if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType))
		{
			UpdateButtonTextureColor(button, sprite);
		}
		else if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType))
		{
			UpdateButtonTextureImage(button, sprite);
		}
	}
}


static ES::Engine::Entity CreateFloor(ES::Engine::Core &core)
{
	glm::vec3 floor_position = glm::vec3(0.0f, -8.0f, 0.0f);
	glm::vec3 floor_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	// Rotated slightly in a 12° angle, so that the sphere will roll on it
	glm::quat floor_rotation = glm::rotate(glm::quat(0.0f, 0.0f, 0.0f, 1.0f), glm::radians(12.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	JPH::Vec3 floor_size = JPH::Vec3(10.0f, 1.0f, 10.0f);

	std::shared_ptr<JPH::BoxShapeSettings> floor_shape_settings = std::make_shared<JPH::BoxShapeSettings>(floor_size);
	ES::Engine::Entity floor = core.CreateEntity();

	floor.AddComponent<Object::Component::Transform>(core, floor_position, floor_scale, floor_rotation);
	floor.AddComponent<Physics::Component::RigidBody3D>(core, floor_shape_settings, JPH::EMotionType::Static, Physics::Utils::Layers::NON_MOVING);

	// Add a mesh to it for rendering
	floor.AddComponent<OpenGL::Component::ShaderHandle>(core, "default");
    floor.AddComponent<OpenGL::Component::MaterialHandle>(core, "default");
    floor.AddComponent<OpenGL::Component::ModelHandle>(core, "floor");

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
	floor.AddComponent<Object::Component::Mesh>(core, mesh);
	floor.AddComponent<Game::Terrain>(core);


	return floor;
}

namespace Game
{
    class GameScene : public ES::Plugin::Scene::Utils::AScene {
        inline static int _numScenes = 0;
    
      public:
        GameScene() : ES::Plugin::Scene::Utils::AScene() {}
    
      protected:
        void _onCreate(ES::Engine::Core &core) final
        {
            CreateFloor(core);
            core.RegisterSystem<ES::Engine::Scheduler::Update>(
                Game::PlayerJump,
				ES::Plugin::UI::System::ButtonClick,
				ES::Engine::Entity::RemoveTemporaryComponents,
				ES::Plugin::UI::System::UpdateButtonState,
				UpdateButtonTexture
            );
            core.RegisterSystem<ES::Engine::Scheduler::FixedTimeUpdate>(
                Game::PointCameraToPlayer,
                Game::PlayerMovement
            );
            Game::SpawnPlayer(core);
			auto buttonEntity = core.CreateEntity();
			buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "2DDefault");
			buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::SpriteHandle>(core, "buttonTest");
			buttonEntity.AddComponent<ES::Plugin::OpenGL::Component::Sprite>(core);
			

			buttonEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core);
			buttonEntity.AddComponent<ES::Plugin::UI::Component::BoxCollider2D>(core, glm::vec2(32.f, 32.f));
			auto &buttonComp = buttonEntity.AddComponent<ES::Plugin::UI::Component::Button>(core);
			buttonComp.onClick = [&](ES::Engine::Core &core) {
				ES::Utils::Log::Info("Button clicked!");
			};
			buttonComp.displayType =
				ES::Plugin::UI::Component::DisplayType::TintColor{.normalColor  = ES::Plugin::Colors::Utils::WHITE_COLOR,
																  .hoverColor   = ES::Plugin::Colors::Utils::GRAY_COLOR,
																  .pressedColor = ES::Plugin::Colors::Utils::DARKGRAY_COLOR};
        }
    
        void _onDestroy(ES::Engine::Core &core) final
        {
            
        }
    };
}
