#include <iostream>
#include <sys/mman.h>
#include <SDL2/SDL.h>

#include "timer.h"

int main() {
    // Initialize SDL //

    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cout << "SDL failed to initialized; " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL Window //

    const size_t cWindowWidth = 640, cWindowHeight = 480;

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
        renderer,                       // Host Renderer
        SDL_PIXELFORMAT_RGBA8888,       // Pixel Format (RGBA, 8 bits each)
        SDL_TEXTUREACCESS_STREAMING,    // Texture Type (Streaming)
        cWindowWidth,                   // Texture Width
        cWindowHeight );                // Texture Height

    // Create and Load Texture //

    size_t cTextureDataBytes = sizeof(uint32_t) * cWindowWidth * cWindowHeight;

    uint32_t* textureData = (uint32_t*)mmap(
        nullptr,                        // Memory Start Address
        cTextureDataBytes,              // Allocation Length (Bytes)
        PROT_READ | PROT_WRITE,         // Protection Flags (Read/Write)
        MAP_ANONYMOUS | MAP_PRIVATE,    // Map Options (In-Memory, Private to Process)
        -1,                             // File Descriptor
        0 );                            // File Offset

    auto loadTexture = [ &texture, &textureData, &cWindowWidth, &cWindowHeight ]
            ( const size_t xOff, const size_t yOff ) {
        for( int y = 0; y < cWindowHeight; y++ ) {
            for( int x = 0; x < cWindowWidth; x++ ) {
                int i = x + y * cWindowWidth;
                textureData[i] = 0;
                textureData[i] |= (uint8_t)( 0x00 ) << 24;    // red
                textureData[i] |= (uint8_t)( y+yOff ) << 16;  // green
                textureData[i] |= (uint8_t)( x+xOff ) << 8;   // blue
                textureData[i] |= (uint8_t)( 0xFF ) << 0;     // alpha
            }
        }

        return SDL_UpdateTexture( texture, nullptr,
            (void*)textureData, sizeof(uint32_t)*cWindowWidth );
    };

    // Update and Render Application //

    timer t( 60 );

    size_t xOffset = 0, yOffset = 0;

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

                if( loadTexture(xOffset, yOffset) != 0 ) {
                    std::cout << "SDL failed to load texture." << std::endl;
                    isRunning = false;
                }

                SDL_RenderClear( renderer );
                SDL_RenderCopy( renderer, texture, nullptr, nullptr );
                SDL_RenderPresent( renderer );
            }
        }

        xOffset += 1;
        yOffset += 1;

        t.wait();
    }

    // Clean Up SDL Assets and Exit //

    munmap( (void*)textureData, cTextureDataBytes );
    SDL_DestroyTexture( texture );

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
