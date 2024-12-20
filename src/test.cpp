#include <iostream>
#include <SDL.h>

int main() {

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "it worked\n";
    } else {
        std::cout << "didnt work\n";
    }

    return 0;

}
