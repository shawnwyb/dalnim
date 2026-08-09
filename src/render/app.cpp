#include "render/app.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "core/algos/registry.hpp"
#include "core/array_layout.hpp"
#include "core/grid_layout.hpp"
#include "core/parse.hpp"
#include "render/camera.hpp"

namespace dalnim {
namespace {
    constexpr int kWindowWidth = 1180;
    constexpr int kWindowHeight = 620;
    constexpr std::size_t kMaxValues = 50;

    constexpr float kSidebarWidth = 330.0f;
    constexpr float kStageMargin = 40.0f;
    constexpr float kStageTop = 60.0f;
    constexpr float kStageBottom = 60.0f;
    constexpr float kMinScale = 0.12f;
    constexpr float kSmallestReadableFont = 9.0f;

    // Core spaces boxes kBoxSpacing apart; a box covers most of that gap.
    constexpr float kBoxUnits = 48.0f;
    constexpr float kShortestBar = 0.18f;
    constexpr double kCellUnits = 48.0;
    constexpr double kCellPitch = 54.0;

    // A run is stretched or squeezed towards this long, so three events and three
    // hundred are both watchable without touching the speed control.
    constexpr double kTargetSeconds = 8.0;
    constexpr double kFastestEvent = 0.05;
    constexpr double kSlowestEvent = 0.60;

    constexpr ImU32 kInk = IM_COL32(236, 236, 236, 255);
    constexpr ImU32 kEdge = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 kLit = IM_COL32(250, 204, 21, 255);
    constexpr ImU32 kLitEdge = IM_COL32(255, 232, 130, 255);
    constexpr ImU32 kEmpty = IM_COL32(20, 20, 20, 255);
    constexpr ImU32 kWall = IM_COL32(88, 88, 88, 255);

    enum class Mode { Array, Grid };

    Mode mode_of(std::size_t i) {
        return wants_array(algorithms()[i]) ? Mode::Array : Mode::Grid;
    }

    // ImGui's dark theme with its blue accents replaced by greys.
    void apply_theme() {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameBorderSize = 1.0f;
        style.WindowPadding = ImVec2(16.0f, 16.0f);
        style.ItemSpacing = ImVec2(8.0f, 8.0f);

        const ImVec4 black{0.00f, 0.00f, 0.00f, 1.00f};
        const ImVec4 dim{0.16f, 0.16f, 0.16f, 1.00f};
        const ImVec4 lift{0.28f, 0.28f, 0.28f, 1.00f};
        const ImVec4 white{1.00f, 1.00f, 1.00f, 1.00f};

        ImVec4* c = style.Colors;
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
        std::size_t grid_start = 0;
        std::size_t algorithm = 0;
        Mode mode = Mode::Array;
        std::vector<int> values;
        ArrayAnimation anim;
        GridAnimation grid_anim;
        double t = 0.0;
        float speed = 1.0f;
        bool playing = true;
        bool truncated = false;
    };

    const EventLog& log_of(const AppState& state) {
        return state.mode == Mode::Array ? state.anim.log : state.grid_anim.log;
    }

    // Time is counted in events, so the duration is simply how many there are.
    double total_events(const AppState& state) {
        return state.mode == Mode::Array ? state.anim.duration : state.grid_anim.duration;
    }

    double seconds_per_event(const AppState& state) {
        const double count = total_events(state);
        if (count <= 0.0) {
            return kSlowestEvent;
        }
        const double fitted = std::clamp(kTargetSeconds / count, kFastestEvent, kSlowestEvent);
        return fitted / static_cast<double>(state.speed);
    }

    std::optional<std::size_t> current_event(const AppState& state) {
        const std::size_t count = log_of(state).size();
        if (count == 0) {
            return std::nullopt;
        }
        const auto k = static_cast<std::size_t>(std::floor(state.t));
        return k >= count ? count - 1 : k;
    }

    void rebuild(AppState& state) {
        const Algorithm& algo = algorithms()[state.algorithm];
        state.mode = mode_of(state.algorithm);
        state.t = 0.0;
        state.playing = true;

        if (const auto* run = std::get_if<GridAlgorithm>(&algo.run)) {
            const Grid grid = parse_grid(state.grid_input);
            if (state.grid_start >= grid.cells.size()) {
                state.grid_start = 0;
            }
            state.grid_anim = build_grid_animation(grid, (*run)(grid, state.grid_start));
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

    void step(AppState& state, int delta) {
        const double edge = delta > 0 ? std::floor(state.t) + 1.0 : std::ceil(state.t) - 1.0;
        state.t = std::clamp(edge, 0.0, total_events(state));
        state.playing = false;
    }

    void toggle_play(AppState& state) {
        state.playing = !state.playing;
        if (state.playing && state.t >= total_events(state)) {
            state.t = 0.0;
        }
    }

    // The drawing area is whatever the sidebar does not cover.
    void stage(float& left, float& width) {
        left = kSidebarWidth;
        width = ImGui::GetIO().DisplaySize.x - kSidebarWidth;
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

    void draw_centred_label(ImDrawList* draw, const std::string& text, float font_size,
                            ImVec2 box_top_left, float box_width, float box_height, ImU32 colour) {
        if (font_size < kSmallestReadableFont) {
            return;
        }
        ImFont* font = ImGui::GetFont();
        const ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());
        if (size.x > box_width || size.y > box_height) {
            return;
        }
        const ImVec2 at{box_top_left.x + (box_width - size.x) * 0.5f,
                        box_top_left.y + (box_height - size.y) * 0.5f};
        draw->AddText(font, font_size, at, colour, text.c_str());
    }

    void draw_bars(const AppState& state) {
        const std::size_t count = state.values.size();
        if (count == 0) {
            return;
        }

        float left = 0.0f;
        float width = 0.0f;
        stage(left, width);

        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        Camera camera = fit_row(row_span(count), width, kStageMargin, kMinScale);
        camera.origin_x += left;

        const float bar_width = camera.length(kBoxUnits);
        const float baseline = screen.y - kStageBottom;
        const float tallest = baseline - kStageTop;
        const float font_size = ImGui::GetFontSize() * camera.scale;

        const auto bounds = std::minmax_element(state.values.begin(), state.values.end());
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        const ComparePair* comparing = compare_at(state.anim, state.t);

        for (std::size_t i = 0; i < count; ++i) {
            const bool lit = comparing != nullptr &&
                             (comparing->box_a == i || comparing->box_b == i);

            const float x = camera.x(state.anim.x[i].sample(state.t));
            const float height = tallest * height_fraction(state.values[i], *bounds.first, *bounds.second);
            const ImVec2 top_left{x, baseline - height};
            const ImVec2 bottom_right{x + bar_width, baseline};

            draw->AddRectFilled(top_left, bottom_right, lit ? kLit : kInk, 4.0f * camera.scale);
            draw->AddRect(top_left, bottom_right, lit ? kLitEdge : kEdge,
                          4.0f * camera.scale, 0, lit ? 2.5f : 1.0f);

            draw_centred_label(draw, std::to_string(state.values[i]), font_size,
                               top_left, bar_width, height, IM_COL32_BLACK);
        }
    }

    ImU32 cell_fill(int value, bool lit) {
        if (lit) {
            return kLit;
        }
        if (value == 0) {
            return kEmpty;
        }
        return value == 1 ? kInk : kWall;
    }

    // Returns the cell the mouse just clicked, so the caller can move the start there.
    std::optional<std::size_t> draw_grid(const AppState& state) {
        const Grid& grid = state.grid_anim.initial;
        if (grid.width == 0 || grid.height == 0) {
            return std::nullopt;
        }

        float left = 0.0f;
        float width = 0.0f;
        stage(left, width);

        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        const double gap = kCellPitch - kCellUnits;
        const double span_x = static_cast<double>(grid.width) * kCellPitch - gap;
        const double span_y = static_cast<double>(grid.height) * kCellPitch - gap;
        Camera camera = fit_box(span_x, span_y, width, screen.y, kStageMargin, kStageTop, kMinScale);
        camera.origin_x += left;

        const std::vector<int> cells = grid_values_at(state.grid_anim, state.t);
        const std::optional<std::size_t> lit = highlighted_at(state.grid_anim, state.t);
        const float side = camera.length(kCellUnits);
        const float font_size = ImGui::GetFontSize() * camera.scale;

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        const ImVec2 mouse = ImGui::GetMousePos();
        const bool mouse_is_ours = !ImGui::GetIO().WantCaptureMouse;
        std::optional<std::size_t> clicked;

        for (std::size_t i = 0; i < cells.size(); ++i) {
            const auto row = static_cast<double>(i / grid.width);
            const auto col = static_cast<double>(i % grid.width);
            const ImVec2 top_left{camera.x(col * kCellPitch), camera.y(row * kCellPitch)};
            const ImVec2 bottom_right{top_left.x + side, top_left.y + side};

            const bool hovered = mouse_is_ours &&
                                 mouse.x >= top_left.x && mouse.x < bottom_right.x &&
                                 mouse.y >= top_left.y && mouse.y < bottom_right.y;
            if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                clicked = i;
            }

            const bool is_lit = lit.has_value() && *lit == i;
            const bool is_start = i == state.grid_start;

            draw->AddRectFilled(top_left, bottom_right, cell_fill(cells[i], is_lit), 4.0f * camera.scale);
            draw->AddRect(top_left, bottom_right, hovered ? kLitEdge : kEdge,
                          4.0f * camera.scale, 0, (is_lit || hovered) ? 2.5f : 1.0f);
            if (is_start) {
                draw->AddRect(ImVec2(top_left.x - 3.0f, top_left.y - 3.0f),
                              ImVec2(bottom_right.x + 3.0f, bottom_right.y + 3.0f),
                              kLitEdge, 5.0f * camera.scale, 0, 1.5f);
            }

            const ImU32 ink = (cells[i] == 0 && !is_lit) ? kEdge : IM_COL32_BLACK;
            draw_centred_label(draw, std::to_string(cells[i]), font_size, top_left, side, side, ink);
        }

        return clicked;
    }

    void draw_sidebar(AppState& state) {
        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(kSidebarWidth, screen.y));
        ImGui::Begin("sidebar", nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::TextUnformatted("dalnim");
        ImGui::Separator();

        ImGui::TextUnformatted("algorithm");
        ImGui::SetNextItemWidth(-1.0f);
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

        ImGui::Spacing();
        if (state.mode == Mode::Array) {
            ImGui::TextUnformatted("values");
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputText("##input", state.input, sizeof(state.input),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                rebuild(state);
            }
            if (state.truncated) {
                ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.13f, 1.0f),
                                   "capped at %zu values", kMaxValues);
            }
        } else {
            ImGui::TextUnformatted("grid");
            ImGui::InputTextMultiline("##grid", state.grid_input, sizeof(state.grid_input),
                                      ImVec2(-1.0f, ImGui::GetTextLineHeight() * 7.5f));
            ImGui::TextDisabled("click a cell to move the start");
        }

        if (ImGui::Button("run", ImVec2(-1.0f, 0.0f))) {
            rebuild(state);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextUnformatted("playback");

        if (ImGui::Button("<", ImVec2(40.0f, 0.0f))) {
            step(state, -1);
        }
        ImGui::SameLine();
        if (ImGui::Button(state.playing ? "pause" : "play", ImVec2(90.0f, 0.0f))) {
            toggle_play(state);
        }
        ImGui::SameLine();
        if (ImGui::Button(">", ImVec2(40.0f, 0.0f))) {
            step(state, 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("restart", ImVec2(-1.0f, 0.0f))) {
            state.t = 0.0;
            state.playing = true;
        }

        float scrubbed = static_cast<float>(state.t);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##t", &scrubbed, 0.0f,
                               static_cast<float>(total_events(state)), "event %.2f")) {
            state.t = scrubbed;
            state.playing = false;
        }

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderFloat("##speed", &state.speed, 0.25f, 4.0f, "speed %.2fx");

        ImGui::Spacing();
        ImGui::Separator();

        const std::size_t count = log_of(state).size();
        const std::optional<std::size_t> now = current_event(state);
        if (now.has_value()) {
            ImGui::Text("event %zu / %zu", *now + 1, count);
            ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.13f, 1.0f), "%s",
                               describe(log_of(state)[*now]).c_str());
        } else {
            ImGui::TextDisabled("no events");
            ImGui::TextDisabled("-");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextDisabled("yellow marks what the algorithm");
        ImGui::TextDisabled("is touching right now.");
        if (state.mode == Mode::Grid) {
            ImGui::TextDisabled("0 is open, 1 is filled, anything");
            ImGui::TextDisabled("else is a wall.");
        }
        ImGui::TextDisabled("space plays, arrows step.");

        ImGui::End();
    }

    void handle_keys(AppState& state) {
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
            toggle_play(state);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
            step(state, 1);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
            step(state, -1);
        }
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
            state.t += dt / seconds_per_event(state);
            if (state.t >= total_events(state)) {
                state.t = total_events(state);
                state.playing = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (state.mode == Mode::Grid) {
            if (const std::optional<std::size_t> clicked = draw_grid(state)) {
                state.grid_start = *clicked;
                rebuild(state);
            }
        } else {
            draw_bars(state);
        }
        draw_sidebar(state);
        handle_keys(state);

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
