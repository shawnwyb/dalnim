#include "core/grid_layout.hpp"
#include <utility>

namespace dalnim {
    GridAnimation build_grid_animation(Grid initial, EventLog log) {
        GridAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.initial = std::move(initial);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> grid_values_at(const GridAnimation& anim, double t) {
        std::vector<int> cells = anim.initial.cells;
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            // A write lands once its own slot of time has elapsed.
            if (t < static_cast<double>(k + 1)) {
                break;
            }
            if (const auto* s = std::get_if<Set>(&anim.log[k])) {
                if (s->index < cells.size()) {
                    cells[s->index] = s->value;
                }
            }
        }
        return cells;
    }

    std::vector<std::optional<MarkKind>> marks_at(const GridAnimation& anim, double t) {
        std::vector<std::optional<MarkKind>> marks(anim.initial.cells.size());
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            if (t < static_cast<double>(k + 1)) {
                break;
            }
            if (const auto* m = std::get_if<Mark>(&anim.log[k])) {
                if (m->index < marks.size()) {
                    marks[m->index] = m->kind;
                }
            } else if (const auto* u = std::get_if<Unmark>(&anim.log[k])) {
                if (u->index < marks.size() && marks[u->index] == u->kind) {
                    marks[u->index].reset();
                }
            }
        }
        return marks;
    }

    std::optional<std::size_t> highlighted_at(const GridAnimation& anim, double t) {
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            const double begin = static_cast<double>(k);
            if (t < begin) {
                break;
            }
            if (t >= begin + 1.0) {
                continue;
            }
            if (const auto* h = std::get_if<Highlight>(&anim.log[k])) {
                return h->index;
            }
        }
        return std::nullopt;
    }
}
