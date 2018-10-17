# llce #

Loop-Live Code Editing Example Based on Handmade Hero (https://handmadehero.org/) and Handmade Penguin (https://github.com/KimJorgensen/sdl_handmade)

## Install ##

The source code for this application was built and tested on an Ubuntu 16.04 Linux machine, and the steps below reflect the installation required for this environment. To replicate this process on an alternate distribution, either install the libraries and their dependencies from source or by using the distro's package management system (a la `apt` for Ubuntu).

Here are the commands required to install all third-part dependencies:

1. `sudo apt-get install clang-3.8 make`: Install build tools (i.e. a C++ compiler and a build command manager).
1. `sudo apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-ttf-2.0-0 libsdl2-ttf-dev`: Install the window management library SDL and its supplemental text library SDL-TTF.
1. `make tty`: Build the console-only demo of loop-live code editing.
1. `make sdl`: Build the graphical demo of loop-live code editing.
