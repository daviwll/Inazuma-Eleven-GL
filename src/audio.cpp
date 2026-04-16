#include "audio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <utility>

AudioPlayer::AudioPlayer() : initialized(false), engine(nullptr), track(nullptr) {}

AudioPlayer::~AudioPlayer() {
    shutdown();
}

bool AudioPlayer::init() {
    if (initialized) {
        return true;
    }

    engine = new ma_engine;
    if (ma_engine_init(nullptr, engine) != MA_SUCCESS) {
        delete engine;
        engine = nullptr;
        return false;
    }

    track = nullptr;
    initialized = true;
    return true;
}

bool AudioPlayer::initLoopingTrack(const std::string& audioPath, float volume) {
    if (!init()) {
        return false;
    }

    if (track != nullptr) {
        return true;
    }

    track = new ma_sound;

    if (ma_sound_init_from_file(engine, audioPath.c_str(), MA_SOUND_FLAG_STREAM, nullptr, nullptr, track) != MA_SUCCESS) {
        delete track;
        track = nullptr;
        return false;
    }

    ma_sound_set_looping(track, MA_TRUE);
    ma_sound_set_volume(track, volume);
    if (ma_sound_start(track) != MA_SUCCESS) {
        ma_sound_uninit(track);
        delete track;
        track = nullptr;
        return false;
    }

    return true;
}

bool AudioPlayer::playOneShot(const std::string& audioPath, float volume) {
    if (!init()) {
        return false;
    }

    ma_sound* sound = new ma_sound;
    if (ma_sound_init_from_file(engine, audioPath.c_str(), MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound) != MA_SUCCESS) {
        delete sound;
        return false;
    }
    
    ma_sound_set_volume(sound, volume);
    
    bool success = (ma_sound_start(sound) == MA_SUCCESS);
    if (!success) {
        ma_sound_uninit(sound);
        delete sound;
        return false;
    }
    
    return true;
}

void AudioPlayer::shutdown() {
    if (track != nullptr) {
        ma_sound_uninit(track);
        delete track;
        track = nullptr;
    }

    if (engine != nullptr) {
        ma_engine_uninit(engine);
        delete engine;
        engine = nullptr;
    }

    initialized = false;
}
