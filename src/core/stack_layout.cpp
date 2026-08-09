#include "core/stack_layout.hpp"
#include <utility>

namespace dalnim {
    StackAnimation build_stack_animation(std::vector<int> input, EventLog log) {
        StackAnimation anim;
        anim.duration = static_cast<double>(log.size());
        anim.input = std::move(input);
        anim.log = std::move(log);
        return anim;
    }

    std::vector<int> stack_at(const StackAnimation& anim, double t) {
        std::vector<int> pile;
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            if (t < static_cast<double>(k + 1)) {
                break;
            }
            if (const auto* push = std::get_if<Push>(&anim.log[k])) {
                pile.push_back(push->value);
            } else if (std::holds_alternative<Pop>(anim.log[k]) && !pile.empty()) {
                pile.pop_back();
            }
        }
        return pile;
    }

    std::optional<std::size_t> cursor_at(const StackAnimation& anim, double t) {
        std::optional<std::size_t> found;
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            if (t < static_cast<double>(k)) {
                break;
            }
            if (const auto* h = std::get_if<Highlight>(&anim.log[k])) {
                found = h->index;
            }
        }
        return found;
    }

    std::optional<std::pair<std::size_t, std::size_t>> comparing_at(const StackAnimation& anim,
                                                                   double t) {
        for (std::size_t k = 0; k < anim.log.size(); ++k) {
            const double begin = static_cast<double>(k);
            if (t < begin) {
                break;
            }
            if (t >= begin + 1.0) {
                continue;
            }
            if (const auto* c = std::get_if<Compare>(&anim.log[k])) {
                return std::pair{c->a, c->b};
            }
        }
        return std::nullopt;
    }
}
