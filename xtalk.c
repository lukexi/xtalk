#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <GL/glew.h>
#include <stdbool.h>
#include <X11/extensions/Xrandr.h>
#include "edid.h"

drm_edid* GetEDIDX11(Display* XDisplay, Window XWindow) {

    int XScreen = DefaultScreen(XDisplay);
    XRRScreenResources* XScreenRes = XRRGetScreenResourcesCurrent(XDisplay, RootWindow(XDisplay, XScreen));
    if (!XScreenRes || XScreenRes->noutput == 0) {
        if (XScreenRes) {
            XRRFreeScreenResources(XScreenRes);
        }
        XScreenRes = XRRGetScreenResources(XDisplay,
            RootWindow(XDisplay, XScreen));
        if (!XScreenRes) {
            printf("Couldn't get screen resources\n");
            return NULL;
        }
    }
    RROutput XOutput = XRRGetOutputPrimary(XDisplay,
            RootWindow(XDisplay, XScreen));

    drm_edid* EDID = NULL;
    for (int OutputIndex = 0; OutputIndex < XScreenRes->noutput; OutputIndex++) {
        RROutput XOutput = XScreenRes->outputs[OutputIndex];

        Atom EDIDProperty = XInternAtom(
            XDisplay,
            RR_PROPERTY_RANDR_EDID,
            false);
        int PropsCount;
        Atom *Props = XRRListOutputProperties(XDisplay, XOutput, &PropsCount);
        for (int PropIndex = 0; PropIndex < PropsCount; PropIndex++) {
            if (Props[PropIndex] == EDIDProperty) {
                unsigned char *PropData;
                int ActualFormat;
                unsigned long NumItems, BytesAfter;
                Atom ActualType;
                if (XRRGetOutputProperty(XDisplay, XOutput, EDIDProperty,
                    0, 256, False, False, AnyPropertyType,
                    &ActualType, &ActualFormat, &NumItems,
                    &BytesAfter, &PropData) == Success) {
                    EDID = calloc(1, sizeof(drm_edid));
                    edid_parse(EDID, PropData, 256);
                    break;
                }
            }
        }
        XFree(Props);
    }
    return EDID;
}

drm_edid* GetWindowDisplayEDID(SDL_Window* SDLWindow) {
    SDL_SysWMinfo WMInfo;
    SDL_VERSION(&WMInfo.version);
    SDL_GetWindowWMInfo(SDLWindow, &WMInfo);

    Display* XDisplay = WMInfo.info.x11.display;
    Window XWindow = WMInfo.info.x11.window;

    return GetEDIDX11(XDisplay, XWindow);
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

        printf("Got a display with bounds %ix%i %ix%i\n",
            DisplayBounds.x, DisplayBounds.y,
            DisplayBounds.w, DisplayBounds.h);

        Windows[DisplayIndex] = Window;

        drm_edid* EDID = GetWindowDisplayEDID(Window);
        printf("Found monitor %s %s %s %s\n",
                        EDID->MonitorName,
                        EDID->SerialNumber,
                        EDID->EISAID,
                        EDID->PNPID);
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
