#pragma once

#include <chrono>
#include <string>

namespace moe {
class Timer {
public:
    Timer()
        : _isStarted { false },
          _isPaused  { false }
        { }


    void start() {
        _start = std::chrono::high_resolution_clock::now();
        _isStarted = true;
    }

    void stop() {
        _isStarted = false;
        _isPaused = false;
    }

    void pause() {
        if (!_isStarted) { return; }
        _isPaused = true;
        _pause = std::chrono::high_resolution_clock::now();
    }

    void unpause() {
        if (!_isStarted || !_isPaused) { return; }
        _isPaused = false;
        _totalPause += std::chrono::high_resolution_clock::now() - _pause;
    }

    /**
     * @return the time elapsed since the timer is on in miliseconds
     */
    int elapsed() const {

        if (!_isStarted) { return 0; }

        if (_isPaused) {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                    _start - _pause - _totalPause).count();
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - _start /*- _totalPause*/).count();
    }

    /**
     * @return the time elapsed since the timer is on, formatted as string in the form:
     *
     * [ minutes ] : [ seconds ] : [ milliseconds ]
     */
    std::string elapsedString() {

        int elapsedMS = elapsed();

        int minutes = elapsedMS / (1000 * 60);
        int seconds = elapsedMS / 1000 - (minutes * 60);
        int ms = elapsedMS % 1000;

        return std::to_string(minutes) + ":" + std::to_string(seconds) + ":" + std::to_string(ms);

    }

    /**
     *  @return the time elapsed since the last call to delta() in milliseconds
     *  @todo will not work the first time called
     */
    int delta() {

        if (_isPaused || !_isStarted) {
            return 0;
        }
        auto result = std::chrono::high_resolution_clock::now() - _delta;
        _delta = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(result).count();
    }

    bool isOn() const       { return _isStarted; }
    bool isPaused() const   { return _isPaused; }

private:
    bool _isPaused;
    bool _isStarted;

    std::chrono::high_resolution_clock::time_point  _start;
    std::chrono::high_resolution_clock::time_point  _delta;
    std::chrono::high_resolution_clock::time_point  _pause;
    std::chrono::high_resolution_clock::duration    _totalPause;
};
}