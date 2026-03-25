#include "audio/sound_system.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#if defined __APPLE__ || _MSC_VER
#include <SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

namespace NN::Audio {

  SoundSystem::SoundSystem() {}

  SoundSystem::~SoundSystem() {
    cleanup();
  }

  bool SoundSystem::setup(int frequency, int channels, int chunkSize) {
    if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, chunkSize) < 0) {
      std::cerr << "SDL_mixer could not initialize: " << Mix_GetError() << std::endl;
      return false;
    }

    // Allocate mixing channels for sound effects
    Mix_AllocateChannels(32);

    initialized = true;
    return true;
  }

  void SoundSystem::cleanup() {
    if (!initialized) return;

    for (auto &[name, chunk] : sounds) {
      if (chunk) Mix_FreeChunk(chunk);
    }
    sounds.clear();

    for (auto &[name, mus] : music) {
      if (mus) Mix_FreeMusic(mus);
    }
    music.clear();

    Mix_CloseAudio();
    initialized = false;
  }

  // Sound effects

  int SoundSystem::loadSound(const std::string &name, const std::string &filepath) {
    Mix_Chunk *chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
      std::cerr << "Failed to load sound '" << name << "' from " << filepath
                << ": " << Mix_GetError() << std::endl;
      return -1;
    }
    sounds[name] = chunk;
    return 0;
  }

  void SoundSystem::playSound(const std::string &name, int loops) {
    if (!initialized || !enabled) return;

    auto it = sounds.find(name);
    if (it == sounds.end()) return;

    Mix_VolumeChunk(it->second, masterSoundVolume);
    Mix_PlayChannel(-1, it->second, loops);
  }

  void SoundSystem::playSoundAt(const std::string &name,
                                 double listenerX, double listenerY,
                                 double sourceX, double sourceY,
                                 double maxDist) {
    if (!initialized || !enabled) return;

    auto it = sounds.find(name);
    if (it == sounds.end()) return;

    double dx = sourceX - listenerX;
    double dy = sourceY - listenerY;
    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist > maxDist) return; // Too far away

    // Calculate volume based on distance (linear falloff)
    double volumeScale = 1.0 - (dist / maxDist);
    volumeScale = std::max(0.0, std::min(1.0, volumeScale));
    int volume = static_cast<int>(masterSoundVolume * volumeScale);

    Mix_VolumeChunk(it->second, volume);

    // Calculate panning (left/right based on relative angle)
    // Simple stereo panning based on x-offset
    double angle = std::atan2(dy, dx);
    // Normalize to 0-255 range for SDL_mixer panning
    // 0 = hard left, 127 = center, 255 = hard right
    int pan = 127 + static_cast<int>(127.0 * std::sin(angle));
    pan = std::max(0, std::min(255, pan));

    int channel = Mix_PlayChannel(-1, it->second, 0);
    if (channel >= 0) {
      Mix_SetPanning(channel, 255 - pan, pan);
    }
  }

  void SoundSystem::setSoundVolume(const std::string &name, int volume) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
      Mix_VolumeChunk(it->second, volume);
    }
  }

  void SoundSystem::setMasterSoundVolume(int volume) {
    masterSoundVolume = std::max(0, std::min(128, volume));
  }

  // Music

  int SoundSystem::loadMusic(const std::string &name, const std::string &filepath) {
    Mix_Music *mus = Mix_LoadMUS(filepath.c_str());
    if (!mus) {
      std::cerr << "Failed to load music '" << name << "' from " << filepath
                << ": " << Mix_GetError() << std::endl;
      return -1;
    }
    music[name] = mus;
    return 0;
  }

  void SoundSystem::playMusic(const std::string &name, int loops) {
    if (!initialized || !enabled) return;

    auto it = music.find(name);
    if (it == music.end()) return;

    Mix_PlayMusic(it->second, loops);
    currentMusic = name;
  }

  void SoundSystem::stopMusic() {
    Mix_HaltMusic();
    currentMusic.clear();
  }

  void SoundSystem::pauseMusic() {
    Mix_PauseMusic();
  }

  void SoundSystem::resumeMusic() {
    Mix_ResumeMusic();
  }

  void SoundSystem::setMusicVolume(int volume) {
    Mix_VolumeMusic(std::max(0, std::min(128, volume)));
  }

  bool SoundSystem::isMusicPlaying() const {
    return Mix_PlayingMusic() != 0;
  }

  // Global

  void SoundSystem::stopAll() {
    Mix_HaltChannel(-1);
    stopMusic();
  }

  void SoundSystem::setEnabled(bool enabled) {
    this->enabled = enabled;
    if (!enabled) {
      stopAll();
    }
  }

  bool SoundSystem::isEnabled() const {
    return enabled;
  }

}
