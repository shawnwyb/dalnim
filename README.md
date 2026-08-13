# dalnim

Data structures and algorithms animation engine. An algorithm runs once and records what it did; the recording is then replayed at any point in time, so the whole run is scrubbable rather than only playable forwards.

## Quick start

Requires CMake 3.20+ and a C++20 compiler, and is tested with CMake 3.31 and Apple Clang 17 on macOS. The first configure downloads SDL3, Dear ImGui, and doctest.

```bash
cmake -B build && cmake --build build && ./build/bin/dalnim
```

After that, building and running is enough. CMake re-configures itself whenever `CMakeLists.txt` changes:

```bash
cmake --build build && ./build/bin/dalnim
ctest --test-dir build --output-on-failure   # run the tests
```

To start over, delete the build directory: `rm -rf build`

## Using dalnim

Pick an algorithm, type an input, and watch it run. Space plays and pauses, and the left and right arrows step one event at a time. Both are ignored while a text box or a slider has the keyboard. The timeline can be dragged to any point in the run.

A caption names what is happening, and yellow always marks what the algorithm is touching right now.

The menu groups algorithms under their topic. Each brings the picture that suits it, and the sidebar says what its input box expects:

- **bars**: bubble sort, selection sort
- **stack**: next greater element
- **grid**: flood fill, breadth-first search
- **tree**: depth-first walk
- **linked list**: cycle detection
- **graph**: breadth-first search (edges are undirected)
- **table**: edit distance
- **intervals**: meeting rooms (one start and end per line)

The list is short because it is easy to grow. An algorithm is a plain function that takes its input and returns an `EventLog`. It records what it did and knows nothing about drawing or timing. Write one against a shape in `core/algos/registry.hpp`, add a line to `kAll` in `registry.cpp` beside the others of its topic, and it appears in the menu with scrubbing and captions already working.

## Acknowledgements

- [manim](https://github.com/3b1b/manim): the inspiration for the whole project, though dalnim departs from it by recording events and replaying them rather than mutating objects in place.
- [SDL3](https://github.com/libsdl-org/SDL): window, input, and rendering
- [Dear ImGui](https://github.com/ocornut/imgui): the interface
- [doctest](https://github.com/doctest/doctest): the tests

The three libraries are fetched by CMake on the first configure; none need installing.
