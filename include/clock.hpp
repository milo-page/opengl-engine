#pragma once

#include <SDL3/SDL.h>
#include <string>

class Clock {
    public:
        Clock();

        void update_time();
        void display_fps_title(SDL_Window** window, const std::string* title);
        double get_fps() const;
        double get_time() const;
        double get_delta_time() const;

    private:
        double last_time = 0;
        double current_time = 0;
        double delta_time = 0;
        uint32_t frame_count = 0;
        double fps = 0.0;
        double last_update_time = 0.0;
};