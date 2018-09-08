#include <iostream>
#include <iomanip>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "./mod_class.h"

using namespace std;

#define AudioSampleRate 44100

#ifdef _WIN32
    #define AudioPufferSize (882)    // 882 bei 44.100 Khz
#else
    #define AudioPufferSize (882*2)    // 882 bei 44.100 Khz
#endif

void AudioMix(void* userdat, Uint8 *stream, int length);

MODClass* mod = NULL;

#undef main
int main(int argc, char *argv[])
{
    char* filename;
    TTF_Font* font1;

    char str1[1024];

    int play_row_nr;
    int play_pattern_nr;
    NOTE* note;

    if(argc > 1)
        filename = argv[1];
    else
        filename = NULL;

    mod = new MODClass(filename, AudioSampleRate);

    if(!mod->ModIsLoaded())
        return(0);

    cout << "Demo-01" << endl;
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (TTF_Init() < 0)
    {
        cerr << "Error: TTF not initialize." << endl;
        SDL_Quit();
        return(0);
    }

    font1 = TTF_OpenFont("ConsolaMono-Bold.ttf",32);
    if(font1 == NULL)
    {
        cerr << "Error: TTF not open font." << endl;
        SDL_Quit();
        return(0);
    }

   // TTF_SetFontKerning(font1,1);

    SDL_Color color_fg = {0,0,255,0};
    SDL_Color color_hg = {0,0,0,255};
    //SDL_Surface* sf = TTF_RenderText_Blended(font1,"Hallo Welt! --- G-5 05 f01",color_fg);
    //SDL_Texture* tx = SDL_CreateTextureFromSurface(ren, sf);
    SDL_Surface* sf;
    SDL_Texture* tx;


    /// SLD Audio Installieren (C64 Emulation) ///
    SDL_AudioSpec want,have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = AudioSampleRate;
    want.format = AUDIO_S16;
    want.channels = 2;
    want.samples = AudioPufferSize;
    want.callback = AudioMix;
    want.userdata = NULL;

    if( SDL_OpenAudio(&want,&have) > 0 )
    {
        cerr << "<< ERROR: Fehler beim installieren von SDL_Audio" << endl;
    }

    if (want.format != have.format)
    {
        cerr << "Audio Format \"AUDIO_S16 wird nicht unterstuetzt." << endl;
    }

    SDL_PauseAudio(0);

    mod->ModPlay();

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

                case SDLK_ESCAPE:
                    break;

                default:
                    break;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN){
               // quit = true;
            }
        }

        // Check of change playing pattern
        if(mod->CheckPatternChange(&play_pattern_nr))
        {
            cout << endl << "Pattern Nr: " << play_pattern_nr << endl;
        }

        // Chaeck of change playing pattern_row
        if(mod->CheckPatternRowChange(&play_row_nr))
        {
           note = mod->GetPatternRow(play_pattern_nr, play_row_nr);

           if(note != NULL)
           {
             cout << std::hex << setfill('0') << setw(2) << play_row_nr << "  | ";

             for(int i=0; i<4;i++)
             {
                /// NOTE and Octave output
                if(note->note_number < 12)
                    cout << NOTE_STRING[note->note_number] << (int)note->oktave_number;
                else
                    cout << "...";
                cout << " ";

                /// Samplenumber output
                if((int)note->sample_number > 0)
                {
                    cout << std::hex << setfill('0') << setw(2) << (int)note->sample_number << " ";
                }
                else
                    cout << ".. ";

                /// Effectnumber and Effectdata output
                if(note->effectcommand == 0x00 && note->effectdata == 0x00)
                    cout << "... | ";
                else
                    cout << std::hex << "\033[1;31m" << (int)note->effectcommand << "\033[0m" << setfill('0') << setw(2) << (int)note->effectdata <<  " | ";
                note++;
             }
             cout << endl;
           }
        }

        //cout << std::hex << setfill('0') << setw(2) << play_row_nr << "  | ";
        sprintf(str1, "%.2X | ",play_row_nr);

        sf = TTF_RenderText_Blended(font1,str1,color_fg);
        tx = SDL_CreateTextureFromSurface(ren, sf);
        SDL_FreeSurface(sf);

        SDL_SetRenderDrawColor(ren,clr,clr,clr,0);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren,0,0,0,0);

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

        int w, h;
        SDL_QueryTexture(tx, NULL, NULL, &w, &h);
        SDL_Rect rec1;
        rec1.x = 0;
        rec1.y = 0;
        rec1.w = w;
        rec1.h = h;
        SDL_RenderCopy(ren,tx,&rec1,&rec1);

        SDL_RenderPresent(ren);
        SDL_Delay(1);
    }

    delete mod;
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void AudioMix(void* userdat, Uint8 *stream, int length)
{
    if(mod != NULL)
        mod->FillAudioBuffer((signed short*)stream, length/2);
    else
        for(int i=0; i<length; i++) stream[i] = 0;
}
