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

bool AudioPlayer::initLoopingTrack(const std::string& audioPath) {
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
    if (ma_sound_start(track) != MA_SUCCESS) {
        ma_sound_uninit(track);
        delete track;
        track = nullptr;
        return false;
    }

    return true;
}

bool AudioPlayer::playOneShot(const std::string& audioPath) {
    if (!init()) {
        return false;
    }

    return ma_engine_play_sound(engine, audioPath.c_str(), nullptr) == MA_SUCCESS;
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
