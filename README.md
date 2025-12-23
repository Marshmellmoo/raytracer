# Ray Tracer

A comprehensive ray tracing engine that renders 3D scenes with advanced lighting and shading techniques.

> **Course Project**: This ray tracer was developed as part of a computer graphics course at Brown University, implementing fundamental and advanced ray tracing techniques from scratch.

## Overview

This ray tracer is a physically-based rendering system capable of producing photorealistic images by simulating the behavior of light rays as they interact with scene geometry. It supports a wide range of rendering features and optimizations to produce high-quality output.

## Features

### Core Rendering Capabilities
- **Ray-Scene Intersection Testing**: Accurate ray-geometry intersection detection for multiple primitive shapes
  - Spheres
  - Cubes
  - Cylinders
  - Cones
- **Phong Shading Model**: Realistic illumination using ambient, diffuse, and specular components
- **Recursive Ray Tracing**: Up to configurable depth (default: 4 levels) for global illumination effects

### Advanced Lighting & Materials
- **Shadows**: Accurate shadow casting with ray-traced shadow rays to light sources
- **Reflection**: Specular reflection from shiny surfaces via recursive ray tracing
- **Refraction**: Transparent materials with proper refraction and Fresnel effects
- **Texture Mapping**: Apply 2D textures to 3D surfaces with proper UV coordinate computation
  - Nearest neighbor filtering
  - Bilinear filtering
  - Mip mapping support

### Image Quality & Performance
- **Supersampling (Anti-aliasing)**: Reduce aliasing artifacts through multiple samples per pixel
  - Grid sampling pattern
  - Configurable samples per pixel (default: 64 SPP)
- **Depth of Field**: Camera depth of field effects for cinematic focusing
- **Parallel Rendering**: Multi-threaded rendering for accelerated performance
- **Acceleration Structures**: Spatial acceleration for faster ray-geometry intersection queries

### Utility Features
- **Normal Map Rendering**: Visualize surface normals for debugging
- **Scene File Format**: INI-based configuration for easy scene setup
- **Camera System**: Flexible perspective camera with configurable field of view and transformations

## Rendering Methods

### 1. **Ray Tracing Algorithm**
Casts primary rays from the camera through each pixel into the scene. At each intersection:
- Computes surface normal and texture coordinates
- Evaluates Phong lighting model
- Casts secondary rays for shadows, reflections, and refractions
- Recursively traces until max depth or ray escapes scene

### 2. **Intersection Testing**
- **Ray-Sphere**: Analytical solution using quadratic equations
- **Ray-Box**: Slab intersection method with AABB testing
- **Ray-Cylinder/Cone**: Parametric ray equation solving

### 3. **Illumination (Phong Shading)**
```
Illumination = Ambient + Σ(Diffuse + Specular) for each light
```
Where:
- Ambient: Global illumination constant
- Diffuse: Lambert's cosine law (N · L)
- Specular: Blinn-Phong highlights based on surface roughness

### 4. **Anti-aliasing**
- Generates multiple samples per pixel using grid patterns
- Averages samples to produce smoother edges and reduce aliasing artifacts
- Configurable samples per pixel for quality/performance trade-off

### 5. **Texture Filtering**
- **Nearest Neighbor**: Fast, pixelated appearance
- **Bilinear Interpolation**: Smooth filtering between texel boundaries
- **Mip Mapping**: Pre-computed texture pyramids for efficient minification

### 6. **Advanced Effects**
- **Reflections**: Traces secondary rays in mirror direction (N - 2(N·L)L)
- **Refractions**: Uses Snell's law for transparent surfaces with proper total internal reflection handling
- **Depth of Field**: Jitters rays within camera aperture for focus effects

## Usage

Build the project with CMake:
```bash
mkdir build
cd build
cmake ..
make
```

Run with a configuration file:
```bash
./raytracer config.ini
```

Configuration files specify:
- Scene file path (.obj-like scene format)
- Output image path and resolution
- Rendering parameters (shadows, reflections, supersampling, etc.)

## Sample Outputs

### Ray-Geometry Intersection
Demonstrates ray-primitive intersection testing with multiple 3D shapes:

| Primitive Salad 1 | Primitive Salad 2 |
|---|---|
| ![Primitive Salad 1](student_outputs/intersect/primitive_salad_1.png) | ![Primitive Salad 2](student_outputs/intersect/primitive_salad_2.png) |

### Illumination & Lighting
Examples of Phong shading with various lighting configurations:

**Point Lights:**
| Point Light 1 | Point Light 2 |
|---|---|
| ![Point Light 1](student_outputs/illuminate/required/point_light_1.png) | ![Point Light 2](student_outputs/illuminate/required/point_light_2.png) |

**Spot Lights:**
| Spot Light 1 | Spot Light 2 |
|---|---|
| ![Spot Light 1](student_outputs/illuminate/required/spot_light_1.png) | ![Spot Light 2](student_outputs/illuminate/required/spot_light_2.png) |

**Shadows:**
| Simple Shadow | Shadow Test | Shadow Special Case |
|---|---|---|
| ![Simple Shadow](student_outputs/illuminate/required/simple_shadow.png) | ![Shadow Test](student_outputs/illuminate/required/shadow_test.png) | ![Shadow Special Case](student_outputs/illuminate/required/shadow_special_case.png) |

**Reflections:**
| Basic Reflections | Complex Reflections |
|---|---|
| ![Basic Reflections](student_outputs/illuminate/required/reflections_basic.png) | ![Complex Reflections](student_outputs/illuminate/required/reflections_complex.png) |

### Texture Mapping
Demonstrates texture application on various primitives:

**Sphere Textures:**
| Texture Sphere 1 | Texture Sphere 2 |
|---|---|
| ![Texture Sphere 1](student_outputs/illuminate/required/texture_sphere.png) | ![Texture Sphere 2](student_outputs/illuminate/required/texture_sphere2.png) |

**Cube Textures:**
| Texture Cube 1 | Texture Cube 2 |
|---|---|
| ![Texture Cube 1](student_outputs/illuminate/required/texture_cube.png) | ![Texture Cube 2](student_outputs/illuminate/required/texture_cube2.png) |

**Cylinder Textures:**
| Texture Cylinder 1 | Texture Cylinder 2 | Cylinder Float Precision |
|---|---|---|
| ![Texture Cylinder 1](student_outputs/illuminate/required/texture_cyl.png) | ![Texture Cylinder 2](student_outputs/illuminate/required/texture_cyl2.png) | ![Cylinder Float](student_outputs/illuminate/required/texture_cyl2_float.png) |

**Cone Textures:**
| Texture Cone 1 | Texture Cone 2 |
|---|---|
| ![Texture Cone 1](student_outputs/illuminate/required/texture_cone.png) | ![Texture Cone 2](student_outputs/illuminate/required/texture_cone2.png) |

### Anti-aliasing
Demonstrates supersampling to reduce aliasing artifacts:

| Bilinear Filtering (Cube) |
|---|
| ![Cube Bilinear Better](student_outputs/antialias/cube_bilinear_better.png) |

## Project Structure

- `src/raytracer/`: Core ray tracing engine
- `src/shapes/`: Primitive shape definitions and intersection tests
- `src/camera/`: Camera model and ray generation
- `src/textures/`: Texture sampling and filtering
- `src/utils/`: Scene parsing, image I/O, utility functions
- `glm/`: Mathematics library for vector and matrix operations


