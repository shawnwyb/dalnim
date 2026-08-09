#include "core/array_layout.hpp"
#include <cstddef>
#include <numeric>
#include <utility>

namespace dalnim {
    ArrayAnimation build_array_animation(const std::vector<int>& input,
                                         const EventLog& log) {
        ArrayAnimation anim;
        anim.x.resize(input.size());
        anim.duration = static_cast<double>(log.size());

        for (std::size_t i = 0; i < input.size(); ++i) {
            anim.x[i].add(0.0, static_cast<double>(i) * kBoxSpacing);
        }

        // Slot s holds this box. A Swap names slots, not boxes.
        std::vector<std::size_t> box_at_slot(input.size());
        std::iota(box_at_slot.begin(), box_at_slot.end(), 0);

        for (std::size_t k = 0; k < log.size(); ++k) {
            const double t0 = static_cast<double>(k);
            const double t1 = t0 + 1.0;

            if (const auto* c = std::get_if<Compare>(&log[k])) {
                anim.compares.push_back(ComparePair{
                    .begin = t0,
                    .end = t1,
                    .box_a = box_at_slot[c->a],
                    .box_b = box_at_slot[c->b],
                });
                continue;
            }

            const auto* s = std::get_if<Swap>(&log[k]);
            if (s == nullptr) {
                continue;
            }

            const double x_a = static_cast<double>(s->a) * kBoxSpacing;
            const double x_b = static_cast<double>(s->b) * kBoxSpacing;

            const std::size_t box_a = box_at_slot[s->a];
            const std::size_t box_b = box_at_slot[s->b];

            anim.x[box_a].add(t0, x_a);
            anim.x[box_a].add(t1, x_b);
            anim.x[box_b].add(t0, x_b);
            anim.x[box_b].add(t1, x_a);

            std::swap(box_at_slot[s->a], box_at_slot[s->b]);
        }

        anim.log = log;
        return anim;
    }

    const ComparePair* compare_at(const ArrayAnimation& anim, double t) {
        for (const ComparePair& c : anim.compares) {
            if (t >= c.begin && t < c.end) {
                return &c;
            }
        }
        return nullptr;
    }
}
