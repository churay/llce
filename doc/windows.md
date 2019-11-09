## Windows + Window Management ##

### Multiple Windows: When + Why ###

When structuring an application that contains multiple contexts and/or interfaces,
a decision needs to be made as to whether these individual contexts will be rendered
within a single monolithic application window or within multiple application windows.
There are innumerable differences between these two approaches, but the most important
to consider from the application's perspective are (1) input/output handling and (2)
performance. Multi-window applications allow users and developers greater freedoms
in terms inputs (e.g. per-window input handling, as determined by operating system-managed
window focus) and outputs (separate OpenGL/rendering contexts), but this flexibility
often comes at the cost of performance (typically caused by the additional communication
overhead required for separate window system calls).

With those major trade-offs in mind, it's clear that selecting a window management
style will be informed largely by the style of application being developed. Real-time
applications (e.g. games and simulations) will generally need to use the single-window
approach in order to maintain reasonable performance levels (on a standard CPU, an application
that runs at ~60 fps can be slowed to ~30 fps or worse by introducing an additional
application window). UI-intense applications (e.g. modeling and animation softwares) will
trend toward the multi-window approach to make user submission of complicated and
potentially overloaded keyboard/input shortcuts more manageable.

### Creating Multiple Windows in SDL2 ###

In the event that multiple windows are deemed appropriate for a particular application
(which will generally be unlikely for `llce`-based programs), the following general
SDL2 workflow can be applied:

```
SDL_Init(SDL_INIT_VIDEO);

{ // Initialize OpenGL Context //
  // ... //
}

/// Create Windows ///

const static uint32_t csWindowCount = // ... //;
SDL_Window* windows[csWindowCount] = // ... //;
SDL_GLContext windowGLs[csWindowCount] = // ... //;
vec2i32_t windowDims[csWindowCount] = // ... //;

const auto cInitWindow = [ &windows, &windowGLs, &windowDims ] ( const uint32_t pWindowIdx ) {
    SDL_GL_MakeCurrent( windows[pWindowIdx], windowGLs[pWindowIdx] );
    glViewport( 0, 0, windowDims[pWindowIdx].x, windowDims[pWindowIdx].y );

    { // Configure OpenGL Context //
        // ... //
    }
};

for( uint32_t windowIdx = 0; windowIdx < csWindowCount; windowIdx++ ) {
    windows[windowIdx] = SDL_CreateWindow( // ... // );
    windowGLs[windowIdx] = SDL_GL_CreateContext( windows[windowIdx] );

    { // Configure OpenGL Context //
        // ... //
    }
}

bool32_t isRunning = true;
while( isRunning ) {
    { // Input //
        // ... //
    }

    { // Update //
        // ... //
    }

    { // Render //
        cInitWindow( 0 ); {
            // Render for Window 0 //
        } SDL_GL_SwapWindow( windows[0] );

        // ... //

        cInitWindow( csWindowCount - 1 ); {
            // Render for Window 0 //
        } SDL_GL_SwapWindow( windows[csWindowCount - 1] );
    }

    { // Clean Up //
        // ... //
    }
}

for( uint32_t windowIdx = 0; windowIdx < csWindowCount; windowIdx++ ) {
    SDL_GL_DeleteContext( windowGLs[windowIdx] );
    SDL_DestroyWindow( windows[windowIdx] );
}

SDL_Quit();
```

### Resources ###

The information for this listing was pulled from the following resources:

- [GameDev SE: Why Use Multiple Windows for Games?](https://gamedev.stackexchange.com/a/62834)
- [GameDev SE: Performance Implications of Multiple Windows](https://gamedev.stackexchange.com/a/23159)
- [Stack Overflow: SDL_GL_SwapWindow Performance](https://stackoverflow.com/a/34650710)
