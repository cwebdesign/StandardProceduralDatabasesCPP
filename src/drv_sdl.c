/*
 * drv_sdl.c - A simple SDL graphics driver for SPD.
 *             C.Meli (@), 2/10/24
 *
 *             This driver requires an 8-bit PseudoColor Visual and has no
 *             support for redrawing caused by exposure events.
 *
 * drv_x11.c is written using Xlib calls based on the X11R6 distribution. A
 * sample makefile, makefile.x11 is provided. If your X11 distribution is in
 * the standard directories the c compiler looks in, things should build by
 * doing 'make -f makefile.x11'. If not, you will need to tell the c compiler
 * where the X11 distribution is located. 
 * 
 * Here are some examples you might make to the makefile.
 * 
 * CC=cc -O -I/usr/local/include/X11 -L/usr/local/lib/X11
 * 
 * CFLAGS=-O -I/usr/local/include/X11 
 * LDFLAGS=-L/usr/local/lib/X11
 * 
 * To test things out try 'balls -r 0'.
 * 
 * 
 * X11 Server Visual Support
 * -------------------------
 * 
 * The drv_x11 only works with 8-bit PseudoColor visuals which are very 
 * common on unix workstations or PC's running Linux and the XF86 SVGA X server.
 * Adding support for Monochrome or 4-bit Visuals is easy but I wanted to
 * keep the code as simple as possible. If you need one, send me some email.
 * 
 * Enjoy,
 * C.Meli
 * 
 */

#include "drv.h"
#include <SDL2/SDL.h>
#include <stdio.h>

/* Globals for SDL */
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

/* Window sizes */
unsigned int width = 512;
unsigned int height = 512;
unsigned int depth = 8;

void display_clear()
{
    /* Clear the display with a black color */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void display_init(int xres, int yres, COORD3 bk_color)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    /* Create an SDL window */
    window = SDL_CreateWindow("SDL Driver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xres, yres, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* Create an SDL renderer */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* Set the background color */
    int r = (int)(bk_color[0] * 255);
    int g = (int)(bk_color[1] * 255);
    int b = (int)(bk_color[2] * 255);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // Set background color
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void display_close(int wait_flag)
{
    if (wait_flag) {
        SDL_Event event;
        int done = 0;

        /* Wait for a key press or mouse button event, then exit */
        while (!done) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                    done = 1;
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/* Optional: not currently used for anything */
void display_plot(int x, int y, COORD3 color)
{
    /* Set the color and draw a pixel */
    int r = (int)(color[0] * 255);
    int g = (int)(color[1] * 255);
    int b = (int)(color[2] * 255);

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // Set drawing color
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderPresent(renderer);  // Show the result on the screen
}

void display_line(int x0, int y0, int x1, int y1, COORD3 color)
{
    /* Set the color and draw a line */
    int r = (int)(color[0] * 255);
    int g = (int)(color[1] * 255);
    int b = (int)(color[2] * 255);

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // Set drawing color
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
    SDL_RenderPresent(renderer);  // Show the result on the screen
}

int kbhit()
{
    /* Check if there is a keypress event in the event queue */
    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (state[i]) {
            return 1;  // A key is pressed
        }
    }
    return 0;
}
