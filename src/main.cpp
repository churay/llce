#include <iostream>
#include <SDL2/SDL.h>

#include "timer.h"

int main() {
    // Initialize SDL //

    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cout << "SDL failed to initialized; " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL Window //

    const int cWindowWidth = 640, cWindowHeight = 480;

    SDL_Window* window = SDL_CreateWindow(
        "Loop-Live Code Editing",       // Window Title
        SDL_WINDOWPOS_UNDEFINED,        // Window X Position
        SDL_WINDOWPOS_UNDEFINED,        // Window Y Position
        cWindowWidth,                   // Window Width
        cWindowHeight,                  // Window Height
        SDL_WINDOW_RESIZABLE );         // Window Initialization Flags

    if( window == NULL ) {
        std::cout << "SDL failed to create window; " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, 0 );
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,                 // Host Renderer
        SDL_PIXELFORMAT_RGBA8888,       // Pixel Format (RGBA, 8 bits each)
        SDL_TEXTUREACCESS_STREAMING,    // Texture Type (Streaming)
        cWindowWidth,                   // Texture Width
        cWindowHeight );                // Texture Height

    // Create and Load Texture //

    uint32_t* textureData = new uint32_t[cWindowWidth*cWindowHeight];
    for( int y = 0; y < cWindowHeight; y++ ) {
        for( int x = 0; x < cWindowWidth; x++ ) {
            int i = x + y * cWindowWidth;
            textureData[i] = 0xFF0000FF;
        }
    }

    int textureLoadStatus = SDL_UpdateTexture( texture, nullptr,
        (void*)textureData, sizeof(uint32_t)*cWindowWidth );

    if( textureLoadStatus != 0 ) {
        std::cout << "SDL failed to load texture; " << textureLoadStatus << std::endl;
        return 1;
    }

    // Update and Render Application //

    timer t( 60 );

    bool isRunning = true;
    while( isRunning ) {
        t.split();

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if( event.type == SDL_QUIT ) {
                isRunning = false;
            } else if( event.type == SDL_WINDOWEVENT  && (
                   event.window.event == SDL_WINDOWEVENT_RESIZED ||
                   event.window.event == SDL_WINDOWEVENT_EXPOSED) ) {
                int newWidth, newHeight;
                SDL_GetWindowSize( window, &newWidth, &newHeight );

                SDL_RenderClear( renderer );
                SDL_RenderCopy( renderer, texture, nullptr, nullptr );
                SDL_RenderPresent( renderer );
            }
        }

        t.wait();
    }

    // Clean Up SDL Assets and Exit //

    delete [] textureData;
    SDL_DestroyTexture( texture );

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
