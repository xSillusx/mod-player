#include <iostream>
#include <math.h>
#include <SDL2/SDL.h>

#include "./mod_class.h"

using namespace std;

#undef main
int main(int argc, char *argv[])
{
    char* filename;

    if(argc > 1)
        filename = argv[1];


    MODClass* mod;

    mod = new MODClass(filename);

    SAMPLE* sample0 = mod->GetSample(0);


    cout << "Demo-01" << endl;
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 800, 600, SDL_WINDOW_SHOWN);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;
    bool quit = false;

    uint8_t clr = 200;

    float r = 290;
    float x = 400;
    float y = 300;
    float N=5;



while (!quit)
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT){
            quit = true;
        }
        if (event.type == SDL_KEYUP){
            switch(event.key.keysym.sym)
            {
            case SDLK_PLUS:
                N++;
                break;

            case SDLK_MINUS:
                N--;
                break;
            default:
                break;
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN){
            quit = true;
        }
    }

    SDL_SetRenderDrawColor(ren,clr,clr,clr,0);


    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren,0,0,0,0);

    int px1=0;
    int py1=0+300;

    signed char* data = (signed char*)sample0->data;
    float xmul = sample0->length / 800.0;
    float xpos_data = 0.0;

    for(int i=0; i<sample0->length; i++)
    {
        int px2 = i;
        int py2 = data[(int)xpos_data]*-1+300;
        xpos_data += xmul;

        SDL_RenderDrawLine(ren,px1,py1,px2,py2);

        px1 = px2;
        py1 = py2;
    }

    /*
    float px1=r*cos(0)+x;
    float py1=r*sin(0)+y;

    for(int i=1; i<N; i++)
    {
        float px2=r*cos(360.0 * i/N)+x;
        float py2=r*sin(360.0 * i/N)+y;

        SDL_RenderDrawLine(ren,px1,py1,px2,py2);

        px1 = px2;
        py1 = py2;
    }

    N++;
    if(N==400)
        N=0;
    */

    SDL_RenderPresent(ren);
}

    delete mod;
    SDL_Quit();
    return 0;
}
