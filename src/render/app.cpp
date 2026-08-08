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

    void draw_transport(double duration, double& t, bool& playing) {
        ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("timeline");

        if (ImGui::Button(playing ? "pause" : "play")) {
            playing = !playing;
            if (playing && t >= duration) {
                t = 0.0;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("restart")) {
            t = 0.0;
            playing = true;
        }

        float scrubbed = static_cast<float>(t);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##t", &scrubbed, 0.0f, static_cast<float>(duration), "%.2f s")) {
            t = scrubbed;
            playing = false;
        }

        ImGui::End();
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

    double t = 0.0;
    bool playing = true;
    Uint64 last_tick = SDL_GetTicks();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const Uint64 now = SDL_GetTicks();
        const double dt = static_cast<double>(now - last_tick) / 1000.0;
        last_tick = now;

        if (playing) {
            t += dt;
            if (t >= anim.duration) {
                t = anim.duration;
                playing = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        draw_boxes(values, anim, t);
        draw_transport(anim.duration, t, playing);
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
