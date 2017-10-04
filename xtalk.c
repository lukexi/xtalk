#import <SDL2/SDL.h>
#import <GL/glew.h>

int main(int argc, char const *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    int NumDisplays = SDL_GetNumVideoDisplays();

    SDL_Window** Windows = calloc(NumDisplays, sizeof(SDL_Window*));
    for (int DisplayIndex = 0; DisplayIndex < NumDisplays; DisplayIndex++) {
        SDL_Rect DisplayBounds;
        SDL_GetDisplayBounds(DisplayIndex, &DisplayBounds);
        SDL_Window* Window = SDL_CreateWindow(
            "XTalk",
            DisplayBounds.x, DisplayBounds.y,
            DisplayBounds.w, DisplayBounds.h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
        SDL_GL_SetSwapInterval(0);

        Windows[DisplayIndex] = Window;
    }
    SDL_GLContext GLContext = SDL_GL_CreateContext(Windows[0]);

    FILE* LogFile  = fopen("xtalk.log", "w");

    unsigned int LastFrame = SDL_GetTicks();
    while (1) {
        for (int DisplayIndex = 0; DisplayIndex < NumDisplays; DisplayIndex++) {
            // SDL_GLContext GLContext = Contexts[0];
            SDL_Window* Window = Windows[DisplayIndex];
            SDL_GL_MakeCurrent(Window, GLContext);
            glClearColor(
                sin((float)SDL_GetTicks() / 100.0) * 0.5 + 0.5,
                DisplayIndex,
                0,
                1);
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_GL_SwapWindow(Window);
        }
        unsigned int Now = SDL_GetTicks();
        unsigned int MillisPassed = Now - LastFrame;
        fprintf(LogFile, "Frame time %i\n", MillisPassed);
        LastFrame = Now;
    }

    return 0;
}
