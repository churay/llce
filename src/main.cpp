#include <iostream>
#include <SDL2/SDL.h>

int main() {
    // Initialize SDL //

    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cout << "SDL failed to initialized; " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL Window //

    SDL_Window* window = SDL_CreateWindow( 
        "SDL2 Example",                 // Window Title
        SDL_WINDOWPOS_UNDEFINED,        // Window X Position
        SDL_WINDOWPOS_UNDEFINED,        // Window Y Position
        640,                            // Window Width
        480,                            // Window Height
        SDL_WINDOW_SHOWN                // Window Initialization Flags
    );

    if( window == NULL ) {
        std::cout << "SDL failed to create window; " << SDL_GetError() << std::endl;
        return 1;
    }

    // Render to the SDL Window //

    SDL_Surface* screenSurface = SDL_GetWindowSurface( window );
    SDL_FillRect(
        screenSurface,                                          // Fill Surface
        NULL,                                                   // Fill Rectangle
        SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF )   // Fill Color
    );

    SDL_UpdateWindowSurface( window );
    SDL_Delay( 2000 );

    // Clean Up SDL Assets and Exit //

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
