/* GG is a GUI for SDL and OpenGL.
   Copyright (C) 2003-2008 T. Zachary Laine

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1
   of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
    
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   If you do not wish to comply with the terms of the LGPL please
   contact the author as other terms are available for a fee.
    
   Zach Laine
   whatwasthataddress@gmail.com */

#include "../../GG/SDL2/SDL2GUI.h"

#include <GG/EventPump.h>
#include <GG/WndEvent.h>

#include <cctype>
#include <iostream>


using namespace GG;

namespace {
    Flags<ModKey> GetSDLModKeys()
    {
        Flags<ModKey> retval;
        Uint32 sdl_keys = SDL_GetModState();
        if (sdl_keys & KMOD_LSHIFT) retval |= MOD_KEY_LSHIFT;
        if (sdl_keys & KMOD_RSHIFT) retval |= MOD_KEY_RSHIFT;
        if (sdl_keys & KMOD_LCTRL)  retval |= MOD_KEY_LCTRL;
        if (sdl_keys & KMOD_RCTRL)  retval |= MOD_KEY_RCTRL;
        if (sdl_keys & KMOD_LALT)   retval |= MOD_KEY_LALT;
        if (sdl_keys & KMOD_RALT)   retval |= MOD_KEY_RALT;
        if (sdl_keys & KMOD_LGUI)   retval |= MOD_KEY_LMETA;
        if (sdl_keys & KMOD_RGUI)   retval |= MOD_KEY_RMETA;
        if (sdl_keys & KMOD_NUM)    retval |= MOD_KEY_NUM;
        if (sdl_keys & KMOD_CAPS)   retval |= MOD_KEY_CAPS;
        if (sdl_keys & KMOD_MODE)   retval |= MOD_KEY_MODE;
        return retval;
    }
}

// member functions
SDL2GUI::SDL2GUI(int w/* = 1024*/, int h/* = 768*/, bool calc_FPS/* = false*/, const std::string& app_name/* = "GG"*/) :
    GUI(app_name),
	m_window(0),
    m_app_width(w),
    m_app_height(h)
{}

SDL2GUI::~SDL2GUI()
{ SDLQuit(); }

X SDL2GUI::AppWidth() const
{ return m_app_width; }

Y SDL2GUI::AppHeight() const
{ return m_app_height; }

unsigned int SDL2GUI::Ticks() const
{ return SDL_GetTicks(); }

void SDL2GUI::operator()()
{ GUI::operator()(); }

void SDL2GUI::Exit(int code)
{
    if (code)
        std::cerr << "Initiating Exit (code " << code << " - error termination)";
    SDLQuit();
    exit(code);
}

void SDL2GUI::Enter2DMode()
{ Enter2DModeImpl(Value(AppWidth()), Value(AppHeight())); }

void SDL2GUI::Exit2DMode()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}

SDL2GUI* SDL2GUI::GetGUI()
{ return dynamic_cast<SDL2GUI*>(GUI::GetGUI()); }

Key SDL2GUI::GGKeyFromSDLKey(const SDL_Keysym& key)
{
    Key retval = Key(key.sym);
    bool shift = key.mod & KMOD_SHIFT;
    bool caps_lock = key.mod & KMOD_CAPS;

    // this code works because both SDLKey and Key map (at least
    // partially) to the printable ASCII characters
    if (shift || caps_lock) {
        if (shift != caps_lock && ('a' <= retval && retval <= 'z')) {
            retval = Key(std::toupper(retval));
        } else if (shift) { // the caps lock key should not affect these
            // this assumes a US keyboard layout
            switch (retval) {
            case '`': retval = Key('~'); break;
            case '1': retval = Key('!'); break;
            case '2': retval = Key('@'); break;
            case '3': retval = Key('#'); break;
            case '4': retval = Key('$'); break;
            case '5': retval = Key('%'); break;
            case '6': retval = Key('^'); break;
            case '7': retval = Key('&'); break;
            case '8': retval = Key('*'); break;
            case '9': retval = Key('('); break;
            case '0': retval = Key(')'); break;
            case '-': retval = Key('_'); break;
            case '=': retval = Key('+'); break;
            case '[': retval = Key('{'); break;
            case ']': retval = Key('}'); break;
            case '\\': retval = Key('|'); break;
            case ';': retval = Key(':'); break;
            case '\'': retval = Key('"'); break;
            case ',': retval = Key('<'); break;
            case '.': retval = Key('>'); break;
            case '/': retval = Key('?'); break;
            default: break;
            }
        }
    }
    return retval;
}

void SDL2GUI::SetAppSize(const Pt& size)
{
    m_app_width = size.x;
    m_app_height = size.y;
}

void SDL2GUI::Enter2DModeImpl(int width, int height)
{
    glPushAttrib(GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_TEXTURE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // This sets up the world coordinate space with the origin in the
    // upper-left corner and +x and +y directions right and down,
    // respectively.
    glOrtho(0.0, width, height, 0.0, -100.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void SDL2GUI::SDLInit()
{
//    const SDL_VideoInfo* vid_info = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError();
        Exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 2);

    m_window = SDL_CreateWindow(AppName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Value(m_app_width), Value(m_app_height), SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    if (m_window == 0) {
        std::cerr << "Video mode set failed: " << SDL_GetError();
        Exit(1);
    }

    m_glcontext = SDL_GL_CreateContext(m_window);
    if (m_glcontext == 0) {
        std::cerr << "Create GL Context failed: " << SDL_GetError();
        Exit(1);
    }

    //SDL_EnableUNICODE(1);

    //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    EnableMouseButtonDownRepeat(500 / 2, 30 / 2);

    GLInit();
}

void SDL2GUI::GLInit()
{
    double ratio = Value(m_app_width * 1.0) / Value(m_app_height);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, Value(m_app_width), Value(m_app_height));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, ratio, 1.0, 10.0);

}

void SDL2GUI::HandleSystemEvents()
{
    // handle events
    SDL_Event event;
    while (0 < SDL_PollEvent(&event)) {
        bool send_to_gg = false;
        EventType gg_event = MOUSEMOVE;
        Key key = GGK_UNKNOWN;
        boost::uint32_t key_code_point = 0;
        Flags<ModKey> mod_keys = GetSDLModKeys();
        Pt mouse_pos(X(event.motion.x), Y(event.motion.y));
        Pt mouse_rel(X(event.motion.xrel), Y(event.motion.yrel));

        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            key = GGKeyFromSDLKey(event.key.keysym);
            key_code_point = event.key.keysym.scancode;
            if (key < GGK_NUMLOCK)
                send_to_gg = true;
            gg_event = (event.type == SDL_KEYDOWN) ? KEYPRESS : KEYRELEASE;
            break;
        case SDL_MOUSEMOTION:
            send_to_gg = true;
            gg_event = MOUSEMOVE;
            break;
        case SDL_MOUSEBUTTONDOWN:
            send_to_gg = true;
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:      gg_event = LPRESS; break;
                case SDL_BUTTON_MIDDLE:    gg_event = MPRESS; break;
                case SDL_BUTTON_RIGHT:     gg_event = RPRESS; break;
                case SDL_BUTTON_X1:        gg_event = MOUSEWHEEL; mouse_rel = Pt(X0, Y1); break;
                case SDL_BUTTON_X2:        gg_event = MOUSEWHEEL; mouse_rel = Pt(X0, -Y1); break;
            }
            mod_keys = GetSDLModKeys();
            break;
        case SDL_MOUSEBUTTONUP:
            send_to_gg = true;
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:   gg_event = LRELEASE; break;
                case SDL_BUTTON_MIDDLE: gg_event = MRELEASE; break;
                case SDL_BUTTON_RIGHT:  gg_event = RRELEASE; break;
            }
            mod_keys = GetSDLModKeys();
            break;
        }

        if (send_to_gg)
            HandleGGEvent(gg_event, key, key_code_point, mod_keys, mouse_pos, mouse_rel);
        else
            HandleNonGGEvent(event);
    }
}

void SDL2GUI::HandleNonGGEvent(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_QUIT:
        Exit(0);
        break;
    }
}

void SDL2GUI::RenderBegin()
{ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void SDL2GUI::RenderEnd()
{ SDL_GL_SwapWindow(m_window); }

void SDL2GUI::FinalCleanup()
{}

void SDL2GUI::SDLQuit()
{
    FinalCleanup();
    SDL_Quit();
}

void SDL2GUI::Run()
{
    try {
        SDLInit();
        Initialize();
        EventPump pump;
        pump();
    } catch (const std::invalid_argument& e) {
        std::cerr << "std::invalid_argument exception caught in GUI::Run(): " << e.what();
        Exit(1);
    } catch (const std::runtime_error& e) {
        std::cerr << "std::runtime_error exception caught in GUI::Run(): " << e.what();
        Exit(1);
    } catch (const ExceptionBase& e) {
        std::cerr << "GG exception (subclass " << e.type() << ") caught in GUI::Run(): " << e.what();
        Exit(1);
    }
}

