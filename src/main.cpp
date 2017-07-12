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
        "Loop-Live Code Editing",       // Window Title
        SDL_WINDOWPOS_UNDEFINED,        // Window X Position
        SDL_WINDOWPOS_UNDEFINED,        // Window Y Position
        640,                            // Window Width
        480,                            // Window Height
        SDL_WINDOW_RESIZABLE            // Window Initialization Flags
    );

    if( window == NULL ) {
        std::cout << "SDL failed to create window; " << SDL_GetError() << std::endl;
        return 1;
    }

    // SDL_Surface* screenSurface = SDL_GetWindowSurface( window );

    // Update and Render Application //

    while( true ) {
        SDL_Event event;
        SDL_WaitEvent( &event );
        if( event.type == SDL_QUIT ) {
            break;
        } else if( event.type == SDL_WINDOWEVENT ) {
            if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                size_t w = event.window.data1, h = event.window.data2;
                std::cout << "Window Resized: (" << w << ", " << h << ")" << std::endl;
            }
        }

        /*
        SDL_FillRect(
            screenSurface,                                          // Fill Surface
            NULL,                                                   // Fill Rectangle
            SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF )   // Fill Color
        );

        SDL_UpdateWindowSurface( window );
        SDL_Delay( 2000 );
        */
    }

    // Clean Up SDL Assets and Exit //

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
