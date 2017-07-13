#include <iostream>
#include <SDL2/SDL.h>

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

    SDL_Renderer* screenRenderer = SDL_CreateRenderer( window, -1, 0 );
    SDL_Texture* screenTexture = SDL_CreateTexture(
        screenRenderer,                 // Host Renderer
        SDL_PIXELFORMAT_ARGB8888,       // Pixel Format (ARGB, 8 bits each)
        SDL_TEXTUREACCESS_STREAMING,    // Texture Type (Streaming)
        cWindowWidth,                   // Texture Width
        cWindowHeight );                // Texture Height

    // Create and Load Texture //

    uint32_t* screenTextureData = new uint32_t[cWindowWidth*cWindowHeight];
    for( int y = 0; y < cWindowHeight; y++ ) {
        for( int x = 0; x < cWindowWidth; x++ ) {
            int i = x + y * cWindowWidth;
            screenTextureData[i] = 0xFFFF0000;
        }
    }

    int textureLoadStatus = SDL_UpdateTexture( screenTexture, nullptr,
        (void*)screenTextureData, sizeof(uint32_t)*cWindowWidth );

    if( textureLoadStatus != 0 ) {
        std::cout << "SDL failed to load texture; " << textureLoadStatus << std::endl;
        return 1;
    }

    // Update and Render Application //

    bool isRunning = true;

    while( isRunning ) {
        SDL_Event event;
        SDL_WaitEvent( &event );
        if( event.type == SDL_QUIT ) {
            isRunning = false;
        } else if( event.type == SDL_WINDOWEVENT  && (
               event.window.event == SDL_WINDOWEVENT_RESIZED ||
               event.window.event == SDL_WINDOWEVENT_EXPOSED) ) {
            int newWidth, newHeight;
            SDL_GetWindowSize( window, &newWidth, &newHeight );

            SDL_RenderClear( screenRenderer );
            SDL_RenderCopy( screenRenderer, screenTexture, nullptr, nullptr );
            SDL_RenderPresent( screenRenderer );
        }
    }

    // Clean Up SDL Assets and Exit //

    delete [] screenTextureData;
    SDL_DestroyTexture( screenTexture );

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
