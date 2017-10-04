#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <GL/glew.h>
#include <stdbool.h>
#include <X11/extensions/Xrandr.h>

void GetEDID(Display* XDisplay) {

    Screen* XScreen = ScreenOfDisplay(XDisplay, 0);

    RROutput XOutput = X11_XRRGetOutputPrimary(
        XDisplay, RootWindow(XDisplay, XScreen));

    Atom EDIDProperty = XInternAtom(
        XDisplay,
        RR_PROPERTY_RANDR_EDID,
        false);
    int PropsCount;
    Atom *Props = X11_XRRListOutputProperties(XDisplay, XOutput, &PropsCount);

    for (int PropIndex = 0; PropIndex < PropsCount; PropIndex++) {
        if (Props[PropIndex] == EDIDProperty) {
            printf("Found EDID prop\n");
        }
    }
}

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


    for (int DisplayIndex = 0; DisplayIndex < NumDisplays; DisplayIndex++) {
        SDL_SysWMinfo WMInfo;
        SDL_VERSION(&WMInfo.version);
        SDL_GetWindowWMInfo(Windows[DisplayIndex], &WMInfo);

        Display* Display = WMInfo.info.x11.display;
        printf("Got a display\n");
    }



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
