#include <switch.h>

#include <stdio.h>
#include <ctype.h>

#include <SDL.h>
#include <glad/glad.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

const GLuint WIDTH = 1280, HEIGHT = 720;

SDL_Window *window;
SDL_GLContext context;

static bool init();

u64 hidKeysAllDown()
{
    u8 controller;
    u64 keysDown = 0;

    for(controller = 0; controller < (u8)CONTROLLER_P1_AUTO; controller++) keysDown |= hidKeysDown((HidControllerID)controller);

    return keysDown;
}

void consoleErrorScreen(const char *fmt, ...){
    consoleInit(NULL);
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    printf("\nPress any button to exit.\n");
    while(appletMainLoop()){
        hidScanInput();
        u64 keysDown = hidKeysAllDown();
        if (keysDown && !((keysDown & KEY_TOUCH) || (keysDown & KEY_LSTICK_LEFT) || (keysDown & KEY_LSTICK_RIGHT) || (keysDown & KEY_LSTICK_UP) || (keysDown & KEY_LSTICK_DOWN) || \
            (keysDown & KEY_RSTICK_LEFT) || (keysDown & KEY_RSTICK_RIGHT) || (keysDown & KEY_RSTICK_UP) || (keysDown & KEY_RSTICK_DOWN))) break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
}

static bool init() {
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ){
        consoleErrorScreen("%s: SDL could not initialize! SDL Error: %s", __func__, SDL_GetError());
        success =  false;
    }
    else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow(
                "[glad] GL with SDL",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                WIDTH, HEIGHT,
                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if( window == NULL ){
            consoleErrorScreen("%s: Window could not be created! SDL Error: %s", __func__, SDL_GetError());
            success =  false;
        }
        else {
            context = SDL_GL_CreateContext(window);
            if( context == NULL )
            {
                consoleErrorScreen( "%s: OpenGL context could not be created! SDL Error: %s", __func__, SDL_GetError());
                success =  false;
            }
            else {
                gladLoadGL();
            }
        }
    }
    return success;
}
int main() {

    if ( init() ) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();

        io.Fonts->AddFontDefault();

        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        bool show_demo_window = true;
        bool show_another_window = false;

        int exit = 0;
        while (!exit) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                switch (event.type) {
                    case SDL_QUIT:
                        exit = 1;
                        break;
                    case SDL_KEYUP:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            exit = 1;
                        }
                        break;
                    default:
                        break;
                }
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);

            ImGui::NewFrame();

            ImGui::ShowDemoWindow(&show_demo_window);

            // Show another simple window.
            {
                ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;
                ImGui::End();
            }

            ImGui::Render();
            glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);

            glClearColor(1.0f, 1.0f, 1.0f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            SDL_GL_SwapWindow(window);
            SDL_Delay(1);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(context);
    }
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
    return 0;
}


