#pragma once

#include <chrono>
#include <memory>

class Time {
private:
    static std::unique_ptr<Time> _instance;
    Time() {
        Start();
    }
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point cut;
    unsigned int cut_length = 0;

public:
    ~Time() {}
    static Time& GetInstance() {
        if (!_instance) {
            _instance = std::unique_ptr<Time>(new Time());
        }
        return *_instance;
    }

    Time& Start() {
        start = std::chrono::high_resolution_clock::now();
        cut = std::chrono::high_resolution_clock::now();
        return *this;
    }

    Time& Cut() {
        auto now = std::chrono::high_resolution_clock::now();
        auto delta = now - cut;
        cut_length = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
        cut = now;
        return *this;
    }

    unsigned int Now() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

    unsigned int TimeFromStart() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto delta = now - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
    }

    unsigned int TimeOfCut() const {
        return cut_length;
    }
};

std::unique_ptr<Time> Time::_instance = nullptr;