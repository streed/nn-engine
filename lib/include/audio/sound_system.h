#ifndef NN_SOUND_SYSTEM_H
#define NN_SOUND_SYSTEM_H

#include <string>
#include <unordered_map>

struct Mix_Chunk;
struct Mix_Music;

namespace NN::Audio {

  class SoundSystem {
    public:
      SoundSystem();
      ~SoundSystem();

      bool setup(int frequency = 44100, int channels = 2, int chunkSize = 2048);
      void cleanup();

      // Sound effects (short clips)
      int loadSound(const std::string &name, const std::string &filepath);
      void playSound(const std::string &name, int loops = 0);
      void playSoundAt(const std::string &name, double listenerX, double listenerY,
                       double sourceX, double sourceY, double maxDist = 20.0);
      void setSoundVolume(const std::string &name, int volume); // 0-128
      void setMasterSoundVolume(int volume); // 0-128

      // Music (background tracks)
      int loadMusic(const std::string &name, const std::string &filepath);
      void playMusic(const std::string &name, int loops = -1); // -1 = infinite
      void stopMusic();
      void pauseMusic();
      void resumeMusic();
      void setMusicVolume(int volume); // 0-128
      bool isMusicPlaying() const;

      // Global control
      void stopAll();
      void setEnabled(bool enabled);
      bool isEnabled() const;

    private:
      bool initialized = false;
      bool enabled = true;
      int masterSoundVolume = 128;

      std::unordered_map<std::string, Mix_Chunk*> sounds;
      std::unordered_map<std::string, Mix_Music*> music;
      std::string currentMusic;
  };

}

#endif
