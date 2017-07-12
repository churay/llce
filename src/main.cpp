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

    SDL_Renderer* screenRenderer = SDL_CreateRenderer( window, -1, 0 );

    // Update and Render Application //

    bool isRunning = true;
    bool isWhite = true;

    while( isRunning ) {
        SDL_Event event;
        SDL_WaitEvent( &event );
        if( event.type == SDL_QUIT ) {
            isRunning = false;
        } else if( event.type == SDL_WINDOWEVENT ) {
            if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                size_t w = event.window.data1, h = event.window.data2;
                std::cout << "Window Resized: (" << w << ", " << h << ")" << std::endl;
            } else if( event.window.event == SDL_WINDOWEVENT_EXPOSED ) {
                if( isWhite ) {
                    SDL_SetRenderDrawColor( screenRenderer, 255, 255, 255, 255 );
                } else {
                    SDL_SetRenderDrawColor( screenRenderer, 0, 0, 0, 255 );
                }
                isWhite = !isWhite;
            }

            SDL_RenderClear( screenRenderer );
            SDL_RenderPresent( screenRenderer );
        }
    }

    // Clean Up SDL Assets and Exit //

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
