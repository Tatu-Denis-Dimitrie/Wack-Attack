export module timer;

import <SDL.h>;
import <string>;

export class Timer {
private:
    Uint32 startTime;
    Uint32 elapsedTime;
    bool running;

public:
    Timer() : startTime(0), elapsedTime(0), running(false) {}

    void start() {
        if (!running) {
            startTime = SDL_GetTicks();
            running = true;
        }
    }

    void stop() {
        if (running) {
            elapsedTime += SDL_GetTicks() - startTime;
            running = false;
        }
    }

    void reset() {
        startTime = 0;
        elapsedTime = 0;
        running = false;
    }

    Uint32 getElapsedTime() const {
        if (running) {
            return elapsedTime + (SDL_GetTicks() - startTime);
        }
        return elapsedTime;
    }

    std::string formatTime() const {
        Uint32 totalMilliseconds = getElapsedTime();
        Uint32 seconds = (totalMilliseconds / 1000) % 60;
        Uint32 minutes = (totalMilliseconds / 60000) % 60;
        Uint32 hours = totalMilliseconds / 3600000;

        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", hours, minutes, seconds);
        return std::string(buffer);
    }
};
