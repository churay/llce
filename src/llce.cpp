#include <iostream>
#include <sys/mman.h>
#include <SDL2/SDL.h>

#include "timer.h"
#include "texture.h"

int main() {
    // Initialize SDL //

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ) {
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

    // Create and Load Texture //

    llce::texture* texture = new llce::texture( renderer, cWindowWidth, cWindowHeight );
    auto loadTexture = [ &texture ] ( const size_t xOff, const size_t yOff ) {
        uint32_t* textureData = texture->mData;
        for( int y = 0; y < texture->mHeight; y++ ) {
            for( int x = 0; x < texture->mWidth; x++ ) {
                int i = x + y * texture->mWidth;
                textureData[i] = 0;
                textureData[i] |= (uint8_t)( 0x00 ) << 24;    // red
                textureData[i] |= (uint8_t)( y+yOff ) << 16;  // green
                textureData[i] |= (uint8_t)( x+xOff ) << 8;   // blue
                textureData[i] |= (uint8_t)( 0xFF ) << 0;     // alpha
            }
        }

        return texture->update();
    };

    // Update and Render Application //

    size_t xOffset = 0, yOffset = 0;

    llce::timer t( 60, llce::timer::type::fps );

    bool isRunning = true, doRender = false;
    while( isRunning ) {
        t.split();

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if( event.type == SDL_QUIT ) {
                isRunning = false;
            } else if( event.type == SDL_WINDOWEVENT  && (
                   event.window.event == SDL_WINDOWEVENT_RESIZED ||
                   event.window.event == SDL_WINDOWEVENT_EXPOSED) ) {
                doRender = true;
            }
            // NOTE(JRC): Implement the following code in order to detect
            // when a key is pressed but not held.
            /*
            } else if( event.type == SDL_KEYDOWN ) {
                SDL_Keycode pressedKey = event.key.keysym.sym;
                if( pressedKey == SDLK_q ) {
                    isRunning = false;
                }
            }
            */
        }

        // NOTE(JRC): The movement numbers seem inverted because the values
        // being changed are the offsets for the underlying texture.  By
        // doing a positive offset in Y for example, we offset the texture
        // downward on the screen, giving the appearance of the window
        // "camera" going upwards.
        const uint8_t* keyboardState = SDL_GetKeyboardState( nullptr );
        if( keyboardState[SDL_SCANCODE_Q] ) {
            isRunning = false;
        } if( keyboardState[SDL_SCANCODE_W] ) {
            yOffset += 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_S] ) {
            yOffset -= 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_A] ) {
            xOffset += 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_D] ) {
            xOffset -= 1;
            doRender = true;
        }

        if( doRender ) {
            if( loadTexture(xOffset, yOffset) != 0 ) {
                std::cout << "SDL failed to load texture." << std::endl;
                isRunning = false;
            }

            SDL_RenderClear( renderer );
            SDL_RenderCopy( renderer, texture->mHandle, nullptr, nullptr );
            SDL_RenderPresent( renderer );
        }

        t.split( true );
    }

    // Clean Up SDL Assets and Exit //

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
