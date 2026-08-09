#pragma once

namespace dalnim {
    // A position in the abstract units core lays things out in. The renderer turns
    // these into pixels; nothing here knows how big a pixel is.
    struct Point {
        double x = 0.0;
        double y = 0.0;
    };
}
