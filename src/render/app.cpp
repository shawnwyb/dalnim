#include "render/app.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "core/algos/registry.hpp"
#include "core/grid_layout.hpp"
#include "core/array_layout.hpp"
#include "core/parse.hpp"
#include "render/camera.hpp"

namespace dalnim {
namespace {
    constexpr int kWindowWidth = 900;
    constexpr int kWindowHeight = 420;
    constexpr std::size_t kMaxValues = 50;

    // Core positions boxes kBoxSpacing units apart; a box covers most of that gap.
    constexpr float kBoxUnits = 48.0f;
    constexpr float kSideMargin = 40.0f;
    constexpr float kMinScale = 0.12f;
    constexpr float kTopMargin = 150.0f;
    constexpr float kBottomMargin = 50.0f;
    constexpr float kShortestBar = 0.18f;
    constexpr float kSmallestReadableFont = 9.0f;
    constexpr double kCellUnits = 48.0;
    constexpr double kCellPitch = 54.0;

    enum class Mode { Array, Grid };

    Mode mode_of(std::size_t i) {
        return wants_array(algorithms()[i]) ? Mode::Array : Mode::Grid;
    }

    // ImGui's dark theme with its blue accents replaced by greys.
    void apply_theme() {
        ImGui::StyleColorsDark();
        ImGui::GetStyle().FrameBorderSize = 1.0f;

        const ImVec4 black{0.00f, 0.00f, 0.00f, 1.00f};
        const ImVec4 dim{0.16f, 0.16f, 0.16f, 1.00f};
        const ImVec4 lift{0.28f, 0.28f, 0.28f, 1.00f};
        const ImVec4 white{1.00f, 1.00f, 1.00f, 1.00f};

        ImVec4* c = ImGui::GetStyle().Colors;
        c[ImGuiCol_WindowBg] = c[ImGuiCol_PopupBg] = c[ImGuiCol_TitleBgActive] = black;
        c[ImGuiCol_Button] = c[ImGuiCol_Header] = c[ImGuiCol_FrameBg] = dim;
        c[ImGuiCol_ButtonHovered] = c[ImGuiCol_HeaderHovered] = c[ImGuiCol_FrameBgHovered] = lift;
        c[ImGuiCol_ButtonActive] = c[ImGuiCol_HeaderActive] = c[ImGuiCol_FrameBgActive] = lift;
        c[ImGuiCol_SliderGrab] = c[ImGuiCol_SliderGrabActive] = c[ImGuiCol_CheckMark] = white;
    }

    struct AppState {
        char input[128] = "5, 3, 8, 1, 9, 2";
        char grid_input[512] =
            "0 0 0 0 9 0 0 0\n"
            "0 9 9 0 9 0 9 0\n"
            "0 9 0 0 0 0 9 0\n"
            "0 9 0 9 9 0 9 0\n"
            "0 0 0 9 0 0 0 0\n"
            "9 9 0 9 0 9 9 0";
        int grid_start = 0;
        std::size_t algorithm = 0;
        Mode mode = Mode::Array;
        std::vector<int> values;
        ArrayAnimation anim;
        GridAnimation grid_anim;
        double t = 0.0;
        bool playing = true;
        bool truncated = false;
    };

    double duration_of(const AppState& state) {
        return state.mode == Mode::Array ? state.anim.duration : state.grid_anim.duration;
    }

    void rebuild(AppState& state) {
        const Algorithm& algo = algorithms()[state.algorithm];
        state.mode = mode_of(state.algorithm);
        state.t = 0.0;
        state.playing = true;

        if (const auto* run = std::get_if<GridAlgorithm>(&algo.run)) {
            const Grid grid = parse_grid(state.grid_input);
            const auto start = static_cast<std::size_t>(state.grid_start < 0 ? 0 : state.grid_start);
            state.grid_anim = build_grid_animation(grid, (*run)(grid, start));
            return;
        }

        const auto run = std::get<ArrayAlgorithm>(algo.run);
        state.values = parse_int_list(state.input);
        state.truncated = state.values.size() > kMaxValues;
        if (state.truncated) {
            state.values.resize(kMaxValues);
        }
        state.anim = build_array_animation(state.values, run(state.values));
    }

    double row_span(std::size_t count) {
        return count == 0 ? 0.0 : static_cast<double>(count - 1) * kBoxSpacing + kBoxUnits;
    }

    // Where a value sits between the smallest and largest on screen, 0..1.
    float height_fraction(int value, int lowest, int highest) {
        if (highest == lowest) {
            return 1.0f;
        }
        const float span = static_cast<float>(highest) - static_cast<float>(lowest);
        const float over = static_cast<float>(value) - static_cast<float>(lowest);
        return kShortestBar + (over / span) * (1.0f - kShortestBar);
    }

    void draw_bars(const AppState& state) {
        const std::size_t count = state.values.size();
        if (count == 0) {
            return;
        }

        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        const Camera camera = fit_row(row_span(count), screen.x, kSideMargin, kMinScale);
        const float width = camera.length(kBoxUnits);
        const float baseline = screen.y - kBottomMargin;
        const float tallest = baseline - kTopMargin;
        const float font_size = ImGui::GetFontSize() * camera.scale;

        const auto bounds = std::minmax_element(state.values.begin(), state.values.end());
        const int lowest = *bounds.first;
        const int highest = *bounds.second;

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        ImFont* font = ImGui::GetFont();
        const ComparePair* comparing = compare_at(state.anim, state.t);

        for (std::size_t i = 0; i < count; ++i) {
            const bool lit = comparing != nullptr &&
                             (comparing->box_a == i || comparing->box_b == i);

            const float x = camera.x(state.anim.x[i].sample(state.t));
            const float height = tallest * height_fraction(state.values[i], lowest, highest);
            const ImVec2 top_left{x, baseline - height};
            const ImVec2 bottom_right{x + width, baseline};

            const ImU32 fill = lit ? IM_COL32(250, 204, 21, 255) : IM_COL32(236, 236, 236, 255);
            const ImU32 edge = lit ? IM_COL32(255, 232, 130, 255) : IM_COL32(255, 255, 255, 255);

            draw->AddRectFilled(top_left, bottom_right, fill, 4.0f * camera.scale);
            draw->AddRect(top_left, bottom_right, edge, 4.0f * camera.scale, 0, lit ? 2.5f : 1.0f);

            const std::string label = std::to_string(state.values[i]);
            const ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, label.c_str());
            if (font_size >= kSmallestReadableFont && size.x <= width && size.y * 1.8f <= height) {
                const ImVec2 at{x + (width - size.x) * 0.5f, top_left.y + size.y * 0.4f};
                draw->AddText(font, font_size, at, IM_COL32_BLACK, label.c_str());
            }
        }
    }

    ImU32 cell_fill(int value, bool lit) {
        if (lit) {
            return IM_COL32(250, 204, 21, 255);
        }
        if (value == 0) {
            return IM_COL32(20, 20, 20, 255);
        }
        if (value == 1) {
            return IM_COL32(236, 236, 236, 255);
        }
        return IM_COL32(88, 88, 88, 255);
    }

    void draw_grid(const AppState& state) {
        const Grid& grid = state.grid_anim.initial;
        if (grid.width == 0 || grid.height == 0) {
            return;
        }

        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        const double span_x = static_cast<double>(grid.width) * kCellPitch - (kCellPitch - kCellUnits);
        const double span_y = static_cast<double>(grid.height) * kCellPitch - (kCellPitch - kCellUnits);
        const Camera camera = fit_box(span_x, span_y, screen.x, screen.y,
                                      kSideMargin, kTopMargin, kMinScale);

        const std::vector<int> cells = grid_values_at(state.grid_anim, state.t);
        const std::optional<std::size_t> lit = highlighted_at(state.grid_anim, state.t);
        const float side = camera.length(kCellUnits);
        const float font_size = ImGui::GetFontSize() * camera.scale;

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        ImFont* font = ImGui::GetFont();

        for (std::size_t i = 0; i < cells.size(); ++i) {
            const std::size_t row = i / grid.width;
            const std::size_t col = i % grid.width;
            const ImVec2 top_left{camera.x(static_cast<double>(col) * kCellPitch),
                                  camera.y(static_cast<double>(row) * kCellPitch)};
            const ImVec2 bottom_right{top_left.x + side, top_left.y + side};

            const bool is_lit = lit.has_value() && *lit == i;
            draw->AddRectFilled(top_left, bottom_right, cell_fill(cells[i], is_lit), 4.0f * camera.scale);
            draw->AddRect(top_left, bottom_right, IM_COL32(255, 255, 255, 255),
                          4.0f * camera.scale, 0, is_lit ? 2.5f : 1.0f);

            const std::string label = std::to_string(cells[i]);
            const ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, label.c_str());
            if (font_size >= kSmallestReadableFont && size.x <= side) {
                const ImVec2 at{top_left.x + (side - size.x) * 0.5f,
                                top_left.y + (side - size.y) * 0.5f};
                const ImU32 ink = (cells[i] == 0 && !is_lit) ? IM_COL32_WHITE : IM_COL32_BLACK;
                draw->AddText(font, font_size, at, ink, label.c_str());
            }
        }
    }

    void draw_panel(AppState& state) {
        ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("dalnim");

        ImGui::SetNextItemWidth(-60.0f);
        if (ImGui::BeginCombo("##algorithm", algorithms()[state.algorithm].name)) {
            for (std::size_t i = 0; i < algorithms().size(); ++i) {
                const bool chosen = i == state.algorithm;
                if (ImGui::Selectable(algorithms()[i].name, chosen)) {
                    state.algorithm = i;
                    rebuild(state);
                }
                if (chosen) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (state.mode == Mode::Array) {
            ImGui::SetNextItemWidth(-60.0f);
            if (ImGui::InputText("##input", state.input, sizeof(state.input),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                rebuild(state);
            }
        } else {
            ImGui::InputTextMultiline("##grid", state.grid_input, sizeof(state.grid_input),
                                      ImVec2(-70.0f, ImGui::GetTextLineHeight() * 7.0f));
            ImGui::SetNextItemWidth(-70.0f);
            ImGui::InputInt("start cell", &state.grid_start);
        }
        ImGui::SameLine();
        if (ImGui::Button("run")) {
            rebuild(state);
        }

        if (ImGui::Button(state.playing ? "pause" : "play")) {
            state.playing = !state.playing;
            if (state.playing && state.t >= duration_of(state)) {
                state.t = 0.0;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("restart")) {
            state.t = 0.0;
            state.playing = true;
        }
        ImGui::SameLine();
        if (state.mode == Mode::Grid) {
            ImGui::Text("%zux%zu grid", state.grid_anim.initial.width, state.grid_anim.initial.height);
        } else if (state.truncated) {
            ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.35f, 1.0f),
                               "%zu values (capped at %zu)", state.values.size(), kMaxValues);
        } else {
            ImGui::Text("%zu values", state.values.size());
        }

        float scrubbed = static_cast<float>(state.t);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##t", &scrubbed, 0.0f,
                               static_cast<float>(duration_of(state)), "%.2f s")) {
            state.t = scrubbed;
            state.playing = false;
        }

        ImGui::End();
    }
}

int run_app() {
    AppState state;
    rebuild(state);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("dalnim", kWindowWidth, kWindowHeight,
                                          SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (window == nullptr || renderer == nullptr) {
        SDL_Log("window or renderer failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    ImGui::CreateContext();
    apply_theme();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

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

        if (state.playing) {
            state.t += dt;
            if (state.t >= duration_of(state)) {
                state.t = duration_of(state);
                state.playing = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        if (state.mode == Mode::Grid) {
            draw_grid(state);
        } else {
            draw_bars(state);
        }
        draw_panel(state);
        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
