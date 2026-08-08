#pragma once

namespace dalnim {
    // Bends a 0..1 fraction so motion starts and ends at rest. Endpoints and the
    // midpoint are unchanged, so it never alters where a thing is, only when.
    double smooth(double alpha);
}
