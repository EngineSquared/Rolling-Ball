#include "Sounds.hpp"
#include "resource/SoundManager.hpp"

void Game::RegisterGameSounds(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().RegisterSound("button_click", "asset/sounds/button.mp3");
    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().RegisterSound("ambient_music", "asset/sounds/ambient.mp3", true);
}