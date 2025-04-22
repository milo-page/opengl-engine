// class Clock {
//     public:
//         Clock() {
//             last_time = SDL_GetTicks() / 1000.0;
//         }

//         void update_time() {
//             current_time = SDL_GetTicks() / 1000.0;
//             frame_count++;
//             delta_time = current_time - last_time;
//             last_time = current_time;
//         }

//         void display_fps_title(SDL_Window** window, const std::string* title) {

//             if (current_time - last_update_time >= 0.5f) {
                
//                 fps = double(frame_count) / (current_time - last_update_time);
            
//                 std::string new_title = *title + " - FPS: " + std::to_string(static_cast<int>(round(fps)));
//                 SDL_SetWindowTitle(*window, new_title.c_str());
    
//                 frame_count = 0;
//                 last_update_time = current_time;
//             }
//         }
        
//         double get_fps() {
//             fps = static_cast<double>(frame_count) / delta_time;
//             return fps;
//         }
//         double get_time() {
//             return current_time;
//         }
//         double get_delta_time() {
//             return delta_time;
//         }
//     private:
//         double last_time = 0;
//         double current_time = 0;
//         double delta_time = 0;
//         uint32_t frame_count = 0;
//         double fps = 0.0;
//         double last_update_time = 0.0;
// };