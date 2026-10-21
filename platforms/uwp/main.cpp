#include <Windows.h>
#include <SDL3/SDL.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include <aurora/main.h>

static int bootstrap(int argc, char** argv) {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "true");
    SDL_SetHint(SDL_HINT_JOYSTICK_WGI, "true");
    return aurora_main(argc, argv);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return SDL_RunApp(0, nullptr, bootstrap, nullptr);
}
