#include "render/app.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <cstddef>
#include <string>
#include <vector>

#include "core/algos/bubble_sort.hpp"
#include "core/layout.hpp"

namespace dalnim {
namespace {
    constexpr int kWindowWidth = 900;
    constexpr int kWindowHeight = 420;
    constexpr float kBoxSize = 48.0f;
    constexpr float kOriginX = 80.0f;
    constexpr float kOriginY = 200.0f;

    void draw_boxes(const std::vector<int>& values, const ArrayAnimation& anim, double t) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        for (std::size_t i = 0; i < values.size(); ++i) {
            const float x = kOriginX + static_cast<float>(anim.x[i].sample(t));
            const ImVec2 top_left{x, kOriginY};
            const ImVec2 bottom_right{x + kBoxSize, kOriginY + kBoxSize};

            draw->AddRectFilled(top_left, bottom_right, IM_COL32(56, 78, 122, 255), 6.0f);
            draw->AddRect(top_left, bottom_right, IM_COL32(150, 180, 220, 255), 6.0f);

            const std::string label = std::to_string(values[i]);
            const ImVec2 size = ImGui::CalcTextSize(label.c_str());
            const ImVec2 at{x + (kBoxSize - size.x) * 0.5f, kOriginY + (kBoxSize - size.y) * 0.5f};
            draw->AddText(at, IM_COL32_WHITE, label.c_str());
        }
    }
}

int run_app() {
    const std::vector<int> values{5, 3, 8, 1, 9, 2};
    const ArrayAnimation anim = build_array_animation(values, bubble_sort(values));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("dalnim", kWindowWidth, kWindowHeight, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (window == nullptr || renderer == nullptr) {
        SDL_Log("window or renderer failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    const Uint64 started = SDL_GetTicks();
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const double elapsed = static_cast<double>(SDL_GetTicks() - started) / 1000.0;
        const double t = anim.duration > 0.0 ? SDL_fmod(elapsed, anim.duration + 1.0) : 0.0;

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        draw_boxes(values, anim, t);
        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 24, 26, 32, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
}
