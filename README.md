# Interactive Procedural Texture Generator

A C++ application that generates procedural textures using Classic Perlin Noise. The textures are generated at runtime rather than loaded from image files.

The application includes four presets: **Clouds**, **Marble**, **Fire**, and **Terrain**. They use the same Perlin Noise implementation with different parameters and color mappings.

## How it works

The noise is generated from a grid of pseudo-random gradient vectors. For each sample point, the four surrounding lattice points are found and their gradient contributions are combined using smooth interpolation.

Several octaves of noise can then be added together using fractal Brownian motion (fBm). The `persistence` and `lacunarity` parameters control how the amplitude and frequency change between octaves.

The final noise value is mapped differently for each preset:

* **Clouds** — blends between sky blue and white.
* **Marble** — uses the noise to distort a sine pattern and create vein-like shapes.
* **Fire** — maps the values through a dark-to-bright fire color ramp.
* **Terrain** — maps different value ranges to water, sand, grass, rock, and snow.

## Building

The project uses C++20 and CMake.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
The dependencies are fetched automatically using CMake FetchContent (just stb for now — MiniFB and microui are added in the next stage for the interactive window).

Running
A headless export mode generates PNG previews without opening a window:

./build/perlin_noise --export screenshots
Interactive window mode is landing in the next stage.

Examples
Clouds	Marble
Clouds	Marble
Fire	Terrain
Fire	Terrain
