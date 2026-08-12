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
#include "core/interval_layout.hpp"
#include "core/parse.hpp"
#include "core/stack_layout.hpp"
#include "core/dp_layout.hpp"
#include "core/graph_layout.hpp"
#include "core/list_layout.hpp"
#include "core/tree_layout.hpp"
#include "render/array_view.hpp"
#include "render/grid_view.hpp"
#include "render/interval_view.hpp"
#include "render/stack_view.hpp"
#include "render/dp_view.hpp"
#include "render/graph_view.hpp"
#include "render/list_view.hpp"
#include "render/tree_view.hpp"
#include "render/stage.hpp"

namespace dalnim {
namespace {
    constexpr int kWindowWidth = 1180;
    constexpr int kWindowHeight = 620;
    constexpr std::size_t kMaxValues = 50;

    // A run is stretched or squeezed towards this long, so three events and three
    // hundred are both watchable without touching the speed control.
    constexpr double kTargetSeconds = 8.0;
    constexpr double kFastestEvent = 0.05;
    constexpr double kSlowestEvent = 0.60;

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
        char tree_input[256] = "8 3 10 1 6 . 14 . . 4 7 . . 13 .";
        char graph_input[512] =
            "1 2\n"
            "0 3 4\n"
            "0 5\n"
            "1\n"
            "1 5\n"
            "2 4";
        int graph_start = 0;
        char dp_input[128] = "kitten\nsitting";
        char list_input[256] = "3 2 0 -4 7 9";
        // Deliberately out of order, so the sort has something to do before the
        // overlap check starts. Sorted input makes the first half of the run look
        // like nothing is happening.
        char interval_input[256] =
            "15 20\n"
            "0 30\n"
            "5 10";
        int list_cycle_to = 2;
        std::size_t grid_start = 0;
        std::size_t algorithm = 0;
        View view = View::Bars;
        std::vector<int> values;
        ArrayAnimation anim;
        GridAnimation grid_anim;
        StackAnimation stack_anim;
        TreeAnimation tree_anim;
        ListAnimation list_anim;
        GraphAnimation graph_anim;
        DpAnimation dp_anim;
        IntervalAnimation interval_anim;
        double t = 0.0;
        float speed = 1.0f;
        bool playing = true;
        bool truncated = false;
    };

    const EventLog& log_of(const AppState& state) {
        switch (state.view) {
            case View::Bars: return state.anim.log;
            case View::Grid: return state.grid_anim.log;
            case View::Stack: return state.stack_anim.log;
            case View::Tree: return state.tree_anim.log;
            case View::List: return state.list_anim.log;
            case View::Graph: return state.graph_anim.log;
            case View::Dp: return state.dp_anim.log;
            case View::Intervals: return state.interval_anim.log;
        }
        return state.anim.log;
    }

    // Time is counted in events, so the duration is simply how many there are.
    double total_events(const AppState& state) {
        return static_cast<double>(log_of(state).size());
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
        state.view = algo.view;
        state.t = 0.0;
        state.playing = true;

        if (const auto* run = std::get_if<DpAlgorithm>(&algo.run)) {
            const DpTable table = parse_dp_table(state.dp_input);
            state.dp_anim = build_dp_animation(table, (*run)(table));
            return;
        }

        if (const auto* run = std::get_if<IntervalAlgorithm>(&algo.run)) {
            Intervals intervals = parse_intervals(state.interval_input);
            state.interval_anim = build_interval_animation(intervals, (*run)(intervals));
            return;
        }

        if (const auto* run = std::get_if<GraphAlgorithm>(&algo.run)) {
            const Graph graph = parse_graph(state.graph_input);
            const auto start = static_cast<std::size_t>(state.graph_start < 0 ? 0 : state.graph_start);
            state.graph_anim = build_graph_animation(graph, (*run)(graph, start));
            return;
        }

        if (const auto* run = std::get_if<ListAlgorithm>(&algo.run)) {
            LinkedList list;
            list.values = parse_int_list(state.list_input);
            if (state.list_cycle_to >= 0 &&
                static_cast<std::size_t>(state.list_cycle_to) < list.values.size()) {
                list.cycle_to = static_cast<std::size_t>(state.list_cycle_to);
            }
            state.list_anim = build_list_animation(list, (*run)(list));
            return;
        }

        if (const auto* run = std::get_if<TreeAlgorithm>(&algo.run)) {
            const Tree tree = parse_tree(state.tree_input);
            state.tree_anim = build_tree_animation(tree, (*run)(tree));
            return;
        }

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
        if (state.view == View::Stack) {
            state.stack_anim = build_stack_animation(state.values, run(state.values));
        } else {
            state.anim = build_array_animation(state.values, run(state.values));
        }
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
        if (state.view == View::Dp) {
            ImGui::TextUnformatted("two words, one per line");
            ImGui::InputTextMultiline("##dp", state.dp_input, sizeof(state.dp_input),
                                      ImVec2(-1.0f, ImGui::GetTextLineHeight() * 3.0f));
        } else if (state.view == View::Intervals) {
            ImGui::TextUnformatted("start and end, one per line");
            ImGui::InputTextMultiline("##intervals", state.interval_input,
                                      sizeof(state.interval_input),
                                      ImVec2(-1.0f, ImGui::GetTextLineHeight() * 7.5f));
        } else if (state.view == View::Graph) {
            ImGui::TextUnformatted("edges, one line per node");
            ImGui::InputTextMultiline("##graph", state.graph_input, sizeof(state.graph_input),
                                      ImVec2(-1.0f, ImGui::GetTextLineHeight() * 7.5f));
            ImGui::SetNextItemWidth(-70.0f);
            ImGui::InputInt("start", &state.graph_start);
        } else if (state.view == View::List) {
            ImGui::TextUnformatted("nodes");
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputText("##list", state.list_input, sizeof(state.list_input),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                rebuild(state);
            }
            ImGui::SetNextItemWidth(-70.0f);
            ImGui::InputInt("loops to", &state.list_cycle_to);
            ImGui::TextDisabled("-1 for no loop");
        } else if (state.view == View::Tree) {
            ImGui::TextUnformatted("tree, slot order");
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputText("##tree", state.tree_input, sizeof(state.tree_input),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                rebuild(state);
            }
            ImGui::TextDisabled("a dot means no node there");
        } else if (state.view != View::Grid) {
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
        ImGui::Spacing();
        // Each hint is one line: the sidebar fits about 44 characters, and ImGui
        // leaves anything longer to run off the edge rather than wrapping it.
        ImGui::TextDisabled("yellow is what it touches now.");
        if (state.view == View::Grid) {
            ImGui::TextDisabled("0 open, anything else a wall.");
            ImGui::TextDisabled("dark grey queued, light grey seen.");
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

        switch (state.view) {
            case View::Grid:
                if (const auto clicked = draw_grid(state.grid_anim, state.t, state.grid_start)) {
                    state.grid_start = *clicked;
                    rebuild(state);
                }
                break;
            case View::Stack:
                draw_stack(state.stack_anim, state.t);
                break;
            case View::Tree:
                draw_tree(state.tree_anim, state.t);
                break;
            case View::List:
                draw_list(state.list_anim, state.t);
                break;
            case View::Graph:
                draw_graph(state.graph_anim, state.t);
                break;
            case View::Dp:
                draw_dp(state.dp_anim, state.t);
                break;
            case View::Intervals:
                draw_intervals(state.interval_anim, state.t);
                break;
            case View::Bars:
                draw_array(state.values, state.anim, state.t);
                break;
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
