#include <clock.hpp>

Clock::Clock() {
    last_time = SDL_GetTicks() / 1000.0;
}

void Clock::update_time() {
    current_time = SDL_GetTicks() / 1000.0;
    frame_count++;
    delta_time = current_time - last_time;
    last_time = current_time;
    if (current_time - last_update_time >= 0.5f) {
        fps = static_cast<double>(frame_count) / (current_time - last_update_time);
    }
}

void Clock::display_fps_title(SDL_Window** window, const std::string* title) {

    if (current_time - last_update_time >= 0.5f) {
    
        std::string new_title = *title + " - FPS: " + std::to_string(static_cast<int>(SDL_round(fps)));
        SDL_SetWindowTitle(*window, new_title.c_str());

        frame_count = 0;
        last_update_time = current_time;
    }
}

double Clock::get_fps() const {
    return fps;
}
double Clock::get_time() const {
    return current_time;
}
double Clock::get_delta_time() const {
    return delta_time;
}