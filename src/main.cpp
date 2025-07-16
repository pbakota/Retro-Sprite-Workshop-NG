#include <filesystem>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include "util.h"
#include "menubar.h"
#include "statusbar.h"
#include "animation.h"
#include "sprite_manager.h"
#include "project_sprites.h"
#include "sprite_image.h"
#include "generator.h"
#include "keyboard_shortcuts.h"
#include "tiled_view.h"
#include "generator.h"
#include "ibm_vga_8x16.h"

bool exitApp = false;
bool wantExit = false;
bool show_demo_window = false;

Project project;
StatusBar statusbar;
Animation animation;
SpriteManager spriteManager(&project, &statusbar);
Generator generator(&spriteManager);
ProjectSprites projectSprites(&spriteManager, &statusbar, &project);
SpriteImage spriteImage(&spriteManager, &projectSprites, &statusbar);
MenuBar menubar(&spriteManager, &projectSprites, &generator);
KeyboardShortcuts keyboardShortcuts(&menubar, &projectSprites, &spriteManager);
TiledView tiledView;

SDL_Window *window;
SDL_Renderer *renderer;

// Main code
int main(int ac, char **av)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    spriteManager.configPath = SDL_GetPrefPath("RetroSpriteWorkshop", "Config");
    spriteManager.configFile = std::filesystem::path(std::string(spriteManager.configPath) + "settings.cfg").make_preferred().string();
    // std::cerr << "configFile path: " << spriteManager.configFile << std::endl;
    spriteManager.LoadConfig();

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(appTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }

    if(!SDL_RenderTargetSupported(renderer)) {
        SDL_Log("Error render target is not supported");
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io.WantCaptureKeyboard = true;
    io.IniFilename = strdup(std::filesystem::path(std::string(spriteManager.configPath) + "rswng.ini").make_preferred().string().c_str());
    // std::cerr << "window ini: " << io.IniFilename << std::endl;

    // Custom font
    ImFontConfig fontConfig;
    // fontConfig.OversampleH = 1;
	// fontConfig.OversampleV = 1;
	// fontConfig.PixelSnapH = 1;
    io.Fonts->AddFontFromMemoryCompressedTTF((void*)ibm_vga_compressed_data, ibm_vga_compressed_size, 16.0f, &fontConfig);
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]     = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

    spriteManager.NewProject();
    #ifdef USE_TEST_SPRITE
    for (auto i = 0; i < 63;++i) {
        spriteManager.AppendSprite(new Sprite(2, 32, vformat("soft_works_aircraft_%d", i).c_str()));
    }
    #endif

    if(ac > 1) {
        // Load project
        menubar.Action_OpenProjectAs(av[1]);
    }

    // Main loop
    while (!exitApp)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                if(spriteManager.projectUnsaved) wantExit = true; else exitApp = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(window)) {
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        if(spriteManager.projectUnsaved) wantExit = true; else exitApp = true;
                    break;
                    case SDL_WINDOWEVENT_RESIZED:
                        spriteManager.InvalidateSprites();
                    break;
                }
            }
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        #ifndef EXCLUDE_IMGUI_DEMO
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        #endif

        static ImGuiID dockspace_id = 0;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("P4Tools Main", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        if (dockspace_id == 0)
        {
            dockspace_id = ImGui::GetID("dockspace");

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
            auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.03f, nullptr, &dockspace_id);
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, nullptr, &dockspace_id);
            // auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.15f, nullptr, &dockspace_id);

            // auto sp_dockspace_id = ImGui::GetID("spriteDockspace");
            // auto sp_dock_id_up = ImGui::DockBuilderSplitNode(sp_dockspace_id, ImGuiDir_Up, 0.5f, nullptr, &sp_dockspace_id);
            // auto sp_dock_id_down = ImGui::DockBuilderSplitNode(sp_dockspace_id, ImGuiDir_Down, 0.03f, nullptr, &sp_dockspace_id);

            ImGui::DockBuilderDockWindow("Sprite Project", dock_id_left);
            ImGui::DockBuilderDockWindow("Sprite Image", dockspace_id);
            ImGui::DockBuilderDockWindow("Animation View", dock_id_top);
            ImGui::DockBuilderDockWindow("Status Bar", dock_id_down);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpace(ImGui::GetID("dockspace"), ImVec2(0, 0), /*ImGuiDockNodeFlags_HiddenTabBar |*/ ImGuiDockNodeFlags_AutoHideTabBar);
        ImGui::End();

        keyboardShortcuts.Handle();
        menubar.render();
        projectSprites.render(renderer);
        spriteImage.render();
        statusbar.render();
        if(spriteManager.currentSprite && spriteManager.tiledWindowVisible) {
            tiledView.render(renderer, spriteManager.currentSprite);
        }

        if(wantExit) {
            if(menubar.ProjectNotSaved(&wantExit, true)) {
                exitApp = true;
            }
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        spriteManager.DetectChanges();
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    spriteManager.SaveConfig();
    SDL_Quit();

    return 0;
}
