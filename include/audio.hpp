#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <string>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    bool init();
    bool initLoopingTrack(const std::string& audioPath);
    bool playOneShot(const std::string& audioPath);
    void shutdown();

private:
    bool initialized;
    struct ma_engine* engine;
    struct ma_sound* track;
};

#endif
