# Interactive Procedural Texture Generator — Free Project Report

## Overview

For this project, I built an interactive C++ application that generates procedural textures using Perlin Noise. The textures are generated during runtime instead of being loaded from image files.

The project includes four presets: **Clouds**, **Marble**, **Fire**, and **Terrain**. They all use the same Perlin Noise implementation, but each preset maps the generated noise values to colors in a different way.

The program can run in two modes. The first is a headless export mode that generates PNG images of the presets. The second is an interactive application using MiniFB and `microui`, where the user can change parameters such as Scale, Octaves, Persistence, Lacunarity, the selected preset, and the random seed.

---

## Perlin Noise

I implemented a 2D Perlin Noise generator in C++. The implementation uses a permutation table that is created from a seed. Because of this, using the same seed gives the same noise pattern, while changing the seed produces a different pattern.

For each point `(x, y)`, the algorithm finds the four surrounding lattice points. Each corner has a pseudo-random gradient direction, and I calculate the dot product between that gradient and the offset from the corner to the sample point.

The four values are then interpolated using a quintic fade function. This interpolation is what gives Perlin Noise its smooth transitions instead of the sharp changes that would appear if every point used an independent random value.

I also implemented fractal noise using several octaves. Each octave samples the noise at a higher frequency and with a smaller amplitude. The `Persistence` parameter controls how much the later octaves contribute, while `Lacunarity` controls how quickly the frequency increases.

---

## Texture Presets

After generating the noise value, I map it to a color depending on the selected preset.

Before applying the color mapping, the noise value is normalized to a range suitable for the texture calculations. I also apply a contrast adjustment because the generated values usually stay close to the middle of the theoretical range. Without this adjustment, some of the textures looked too flat.

The four presets work as follows:

* **Clouds** use a blend between sky blue and white. A smooth transition is used so that the result does not look like a simple linear gradient.
* **Marble** uses the noise value to distort a sine pattern. This creates irregular bands that look similar to marble veins.
* **Fire** uses a color ramp that moves from near-black through red, orange, yellow, and finally a pale highlight. The mapping is adjusted so that only a small part of the texture reaches the brightest colors.
* **Terrain** treats the noise value as an elevation. Different ranges are mapped to deep water, shallow water, sand, grass, rock, and snow.

### Preset results

The screenshots below were generated using the export mode with fixed parameters for each preset.

| Preset  | Scale | Octaves | Persistence | Lacunarity | Seed |
| ------- | ----- | ------- | ----------- | ---------- | ---- |
| Clouds  | 4.0   | 5       | 0.50        | 2.0        | 7    |
| Marble  | 3.0   | 4       | 0.55        | 2.2        | 42   |
| Fire    | 5.0   | 6       | 0.60        | 2.0        | 99   |
| Terrain | 4.0   | 6       | 0.50        | 2.0        | 5    |

| Clouds                                            | Marble                                          |
| ------------------------------------------------- | ----------------------------------------------- |
| ![Clouds](screenshots/clouds_scale4_octaves5.png) | ![Marble](screenshots/marble_veins_default.png) |

| Fire                                       | Terrain                                           |
| ------------------------------------------ | ------------------------------------------------- |
| ![Fire](screenshots/fire_high_octaves.png) | ![Terrain](screenshots/terrain_default_seed5.png) |

### Changing the seed

I also compared the Terrain preset using two different seeds while keeping all of the other parameters unchanged.

The Scale, Octaves, Persistence, Lacunarity, elevation thresholds, and color mapping remain the same. The only change is the seed, which changes the gradient pattern used by the Perlin Noise generator.

As a result, the overall type of terrain stays the same, but the locations of the water, land, and higher regions change.

| Seed 5                                                   | Seed 123                                             |
| -------------------------------------------------------- | ---------------------------------------------------- |
| ![Terrain seed 5](screenshots/terrain_default_seed5.png) | ![Terrain seed 123](screenshots/terrain_seed123.png) |

---

## Interactive Application

The interactive version uses a fixed 900×600 MiniFB window. The generated texture is displayed on the left side, and the control panel is displayed on the right.

The interface was implemented using `microui`. It includes preset buttons and sliders for Scale, Octaves, Persistence, and Lacunarity, as well as a control for generating a new random seed.

When one of these values changes, the texture is generated again immediately. This allows the effect of each parameter to be seen while the application is running.

MiniFB provides the window, framebuffer, and input information. I added the connection between MiniFB input and the `microui` controls so that mouse, keyboard, and text events are handled correctly.

I also implemented the rendering of the `microui` interface directly into the same pixel buffer used by the generated texture. This includes the UI rectangles, text, icons, and clipping.

One small implementation issue was that the font and icon atlas supplied with `microui` uses C99 syntax that does not compile directly as C++. To solve this, I compiled that part as a C source file and exposed the atlas data to the rest of the C++ application through a header.

### Result

The screenshots below show the interactive application using the default Clouds preset and the Terrain preset.

| Clouds                                                      | Terrain                                                    |
| ----------------------------------------------------------- | ---------------------------------------------------------- |
| ![UI clouds](screenshots/interactive_ui_clouds_default.png) | ![UI terrain](screenshots/interactive_ui_terrain_live.png) |

Both screenshots use the same Scale, Octaves, Persistence, Lacunarity, and Seed values. Only the selected preset is different.

---

## Additional Improvements

After the main interactive application was working, I added two improvements.

### Active preset highlight

At first, the preset buttons did not clearly show which preset was currently selected. I changed the button style so that the active preset is highlighted in blue.

This makes the interface easier to understand because the user can immediately see which texture preset is being displayed.

### Multithreaded texture generation

Originally, the texture was generated using a single loop over all rows of the image.

Since the whole texture is regenerated whenever a slider changes, this could make the controls feel less responsive, especially when using more octaves.

I changed the generation step so that the rows are divided into ranges and processed by several CPU threads. The number of threads is based on the available hardware threads, with a single-threaded fallback if necessary.

This does not change the generated texture; it only makes the regeneration faster and keeps the interface more responsive.

---

## Build and Tooling

* **Language:** C++20
* **Build system:** CMake
* **MiniFB:** used for the application window, framebuffer, and input
* **microui:** used for the interactive control panel
* **stb:** used for PNG export
* **C++ threading library:** used for multithreaded texture generation

The dependencies are handled through CMake, so they do not need to be added manually.

The project also includes a headless export mode:

```sh
./build/perlin_noise --export <dir>
```

This mode generates PNG previews without opening the interactive window. I used it to generate the preset screenshots shown in this report.

For Windows, I also added a `build_and_run.ps1` script that configures the project with the Visual Studio C++ toolchain, builds it using Ninja, and runs the application.

---

## Conclusion

The project started from the Perlin Noise technique covered in the course and developed it into a complete interactive application.

The part I found most interesting was that the noise generation itself stays mostly the same, while the final appearance changes depending on how the values are processed and mapped to colors. The same basic noise can therefore be used for textures that look very different from each other.

I also added the interactive controls and multithreaded generation so that the parameters can be changed while the program is running without making the interface feel slow.

If I continued developing the project, I would make the application window resizable and adapt the texture area and control panel automatically to the new window size.
