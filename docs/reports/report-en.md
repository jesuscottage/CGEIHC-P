# Our World: Interactive 3D Simulation of Global Warming at the North Pole

## Final Project Report

---

**Course**: Computer Graphics and Human-Computer Interaction (CGEIHC)
**Group**: Theory 2
**Instructor**: Dr. Sergio Teodoro Vite
**Institution**: National Autonomous University of Mexico — Faculty of Engineering
**Semester**: 2026-2

### Team

| Name | Student ID |
|------|-----------|
| Hernandez Cabanas Jesus | 422062224 |
| Navarro Nuno Juan Pablo | 319086779 |
| Macias Nino Carmen Violeta | 318285450 |
| Martinez Jimenez Israel | 312032184 |

**Submission date**: May 2026

---

## Abstract

An interactive 3D real-time graphics system was developed that simulates a virtual educational museum set at the North Pole, aiming to raise awareness about global warming and its effects on Arctic ecosystems. The system was implemented in C++17 using OpenGL 3.3 Core Profile with a programmable pipeline (GLSL), integrating Blinn-Phong lighting with Fresnel reflection, environment mapping via cubemap, three types of animation (basic, keyframe, and procedural), and a guided narrative walkthrough. The project runs stably at 59-61 FPS on integrated graphics hardware (AMD Radeon), demonstrating the feasibility of educational real-time computer graphics applications.

**Keywords**: computer graphics, OpenGL, real-time rendering, global warming, virtual museum, human-computer interaction.

---

## 1. Introduction

### 1.1 Problem Context

Global warming represents one of the greatest threats to polar ecosystems. According to the Intergovernmental Panel on Climate Change (IPCC, 2023), the average Arctic temperature has risen at twice the global rate over the past four decades. Arctic sea ice extent has decreased by approximately 13% per decade since 1979. These changes have direct consequences for Arctic wildlife, sea levels, and global weather patterns.

Environmental education is essential for raising awareness about these issues. However, traditional media (text, static images, videos) fail to convey the scale and urgency of climate change in an immersive manner. Interactive 3D environments offer an educational alternative that allows users to visually experience the consequences of global warming.

### 1.2 Project Objective

Design and implement an interactive 3D graphics system simulating a virtual museum at the North Pole, where the user explores educational modules about the causes, consequences, and solutions of global warming, using real-time computer graphics techniques.

### 1.3 Scope

The system includes:
- Real-time 3D rendering with OpenGL 3.3 Core Profile
- Two narrative routes (consequences and solutions) with 7 interactive modules
- Advanced lighting (Blinn-Phong + Fresnel)
- Environment mapping (skybox cubemap)
- Three types of animation (basic, keyframe LERP, procedural)
- Water shader with vertex displacement
- Snow particle system
- User interface with Dear ImGui
- Ambient audio with miniaudio

---

## 2. Theoretical Framework

### 2.1 OpenGL Rendering Pipeline

OpenGL (Open Graphics Library) is a standard API for hardware-accelerated 2D and 3D graphics rendering. Version 3.3 Core Profile removes the fixed-function pipeline and requires the use of programmable shaders in GLSL (OpenGL Shading Language) (de Vries, 2020).

The modern rendering pipeline consists of the following stages:

1. **Vertex Shader**: transforms vertex coordinates from model space to clip space using Model, View, and Projection (MVP) matrices.
2. **Rasterization**: converts geometric primitives into fragments.
3. **Fragment Shader**: computes the final color of each fragment by applying lighting, textures, and effects.
4. **Tests and blending**: depth test, stencil test, and alpha blending.

### 2.2 Blinn-Phong Lighting Model

The Blinn-Phong model (Blinn, 1977) is an efficient approximation of local illumination that decomposes reflectance into three components: ambient, diffuse (Lambertian), and specular (using the half-vector between light and view directions).

### 2.3 Fresnel Reflection

The Fresnel effect describes how surface reflectance varies with the angle of incidence. Schlick's approximation (1994) provides an efficient evaluation: F(theta) = F0 + (1 - F0)(1 - cos(theta))^5, where F0 is the reflectance at normal incidence (0.02 for water, 0.04 for dielectrics).

### 2.4 Environment Mapping with Cubemaps

A cubemap is a texture composed of 6 square images representing the faces of an infinite cube surrounding the scene, used for skybox rendering and environment reflections. The skybox uses the `gl_Position.xyww` trick to ensure it always renders behind all geometry.

### 2.5 Real-time Animation

Three types of animation were implemented:
1. **Basic animation**: direct transformations (translation, rotation, scaling) applied per frame.
2. **Keyframe animation (LERP/SLERP)**: linear interpolation between initial and final states controlled by parameter t in [0, 1].
3. **Procedural animation**: mathematically generated without predefined keyframes, using sinusoidal functions and noise.

---

## 3. Development

### 3.1 System Architecture

The system is organized in modular layers:

**Technical stack**: C++17, OpenGL 3.3 Core Profile, GLFW 3.4, GLM 1.0.1, Assimp 5.4.3, stb_image, Dear ImGui 1.91.6, miniaudio 0.11+, CMake 3.20+ with FetchContent.

### 3.2 Rendering Pipeline

Per-frame rendering order: opaque museum geometry (textured floor, walls, platforms) -> 3D module scenes (GLB models via Assimp or procedural fallback) -> decorative fauna -> 3D sign panels -> semitransparent water (alpha blended) -> snow particle billboards -> skybox cubemap -> ImGui overlays.

### 3.3 Shaders

Four shaders were implemented:
- **Standard** (Blinn-Phong + Fresnel + hemispheric light + exponential fog)
- **Water** (3-layer sinusoidal vertex displacement + foam + Fresnel reflection)
- **Skybox** (cubemap sampling with depth trick)
- **Unlit** (for billboards and colored signs)

### 3.4 Narrative Module System

Seven interactive modules across two routes:

**Left route (consequences)**: melting iceberg (M1), polar bear habitat loss (M2), city flooding with real building models (M3).

**Right route (solutions)**: wind turbine (M1), electric car with real GLB model (M2), reforestation with real tree model (M3).

**Shared module**: rotating globe (M5) with international agreements.

### 3.5 Real Asset Integration

CC0 (Creative Commons Zero) assets were integrated from authentication-free sources:
- 8 PBR color maps from ambientcg.com
- 6-face PNG skybox cubemap (512x512) from OpenGameArt.org
- 6 GLB models from kenney.nl and KhronosGroup
- 3 audio files from archive.org
- Roboto-Regular.ttf font from Google Fonts

A memory-based loading mechanism (`ReadFileFromMemory`) was implemented as a fallback for Assimp on Windows paths containing Unicode characters.

---

## 4. Results

### 4.1 Performance

| Module | Average FPS | animT at frame 600 | Status |
|--------|------------|-------------------|--------|
| M1_IZQ | 59.97 | 0.891 | Stable |
| M2_IZQ | 59.98 | 1.000 | Stable |
| M3_IZQ | 59.10 | 0.743 | Stable |
| M1_DER | 60.96 | 1.000 | Stable |
| M2_DER | 60.95 | 1.000 | Stable |
| M3_DER | 60.97 | 1.000 | Stable |
| M5 | 60.95 | 0.744 | Stable |

All modules maintain FPS above 59, ensuring a smooth experience on integrated graphics hardware.

### 4.2 Rubric Compliance

| Requirement | Implementation | Status |
|-------------|---------------|--------|
| OpenGL 3.3+ Core Profile | GLSL 330 core in all shaders | Met |
| 2+ lighting techniques | Blinn-Phong + Fresnel | Met |
| 1+ environment mapping | Skybox cubemap with PNG loading | Met |
| 3 types of animation | Basic + Keyframe LERP + Procedural (water, snow) | Met |
| Narrative walkthrough | Title -> 2 routes -> M5 -> Closing | Met |
| Contextual interactions | E key to activate modules by proximity | Met |
| Adequate fluidity | 59-61 stable FPS | Met |

---

## 5. Conclusions

An interactive 3D virtual museum about global warming at the North Pole was successfully implemented, meeting all technical requirements of the course rubric. The system demonstrates the viability of using real-time 3D graphics as an educational tool for environmental topics.

Key technical contributions include a complete rendering pipeline with 4 specialized shaders, CC0 asset integration with robust fallback mechanisms, a narrative module system with keyframe animation, and autonomous verification through programmatic screenshots.

---

## 6. References

Angel, E. & Shreiner, D. (2011). *Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL* (6th ed.). Addison-Wesley.

Blinn, J. F. (1977). Models of light reflection for computer synthesized pictures. *ACM SIGGRAPH Computer Graphics*, 11(2), 192-198.

de Vries, J. (2020). *Learn OpenGL: Graphics Programming*. Retrieved from https://learnopengl.com/

Hearn, D. & Baker, M. P. (2006). *Computer Graphics with OpenGL* (4th ed.). Prentice Hall.

IPCC (2023). *Climate Change 2023: Synthesis Report*. Intergovernmental Panel on Climate Change.

Nielsen, J. (1994). *Usability Engineering*. Morgan Kaufmann Publishers.

Schlick, C. (1994). An inexpensive BRDF model for physically-based rendering. *Computer Graphics Forum*, 13(3), 233-246.
