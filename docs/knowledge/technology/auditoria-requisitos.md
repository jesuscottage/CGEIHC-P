---
title: Auditoría de cumplimiento de requisitos — CGEIHC-P
date: 2026-05-28
category: technology
tags: [rúbrica, auditoría, requisitos, evaluación]
status: complete
---

# Auditoría Exhaustiva de Cumplimiento — CGEIHC-P

**Fecha**: 2026-05-28
**Proyecto**: Calentamiento Global en el Polo Norte

---

## Resumen Ejecutivo

| Ámbito | Requisito | Implementación | Cumplimiento |
|--------|-----------|----------------|--------------|
| **Iluminación** | 2+ técnicas | 5 técnicas (Blinn-Phong, Fresnel Schlick, hemisférica, point lights, especular) | 100% |
| **Environment Mapping** | 1+ técnica | 2 técnicas (cubemap skybox + reflexión Fresnel agua) | 100% |
| **Animaciones** | 3 tipos | 3 tipos completos (básica + keyframe LERP + procedural sinusoidal) | 100% |
| **Recorrido Narrativo** | Punto inicial y final | Vestíbulo → 7 módulos → Cierre con celebración | 100% |
| **Interacciones** | Contextuales | 5 sistemas (tecla E, popups, narración, colisiones, progreso) | 100% |
| **Texturas** | Presentes | Texturas CC0 + galería 24 pinturas + materiales diferenciados | 95% |
| **Audio** | Presente | Ambient loop + narración 7 módulos + celebración | 100% |
| **UI/HUD** | Presente | 5 paneles ImGui (título, contador, debug, narrativo, cierre) | 100% |
| **FPS** | Adecuada | 165 FPS promedio estable | 100% |
| **Entregables** | Ejecutable + Instalador + Video + Reporte | Ejecutable y reportes listos; instalador y video pendientes del equipo | 80% |

**Calificación estimada**: 92-95%

---

## 1. Iluminación (Requisito: 2+ técnicas) — CUMPLE

### Técnicas implementadas en `standard.frag`:

| Técnica | Ubicación | Detalles |
|---------|-----------|---------|
| Blinn-Phong (difuso + especular) | standard.frag:51-65 | NdotL + NdotH^shininess |
| Fresnel Schlick | standard.frag:34-36, water.frag:26-28 | F0 + (1-F0) × (1-cosθ)^5 |
| Luz hemisférica | standard.frag:45-49 | mix(ground, sky, hemi) × 0.55 |
| Point lights (8 máx) | standard.frag:27-84 | Atenuación cuadrática + Fresnel |
| Fill light secundario | standard.frag:53-54 | NdotFill × 0.25 (reflexión hielo) |

### Parámetros de iluminación (main.cpp:231-234):
- Dirección sol: `(-0.2, -0.7, 0.3)` normalizado
- Color luz: `(1.0, 0.97, 0.90)` — blanco cálido
- Color niebla: `(0.72, 0.80, 0.88)` — azul ártico

---

## 2. Environment Mapping (Requisito: 1+ técnica) — CUMPLE

| Técnica | Ubicación | Detalles |
|---------|-----------|---------|
| Cubemap skybox | Skybox.h + skybox.frag | 6 caras PNG 512×512, samplerCube |
| Reflexión Fresnel agua | water.frag:53-71 | skyReflect × viewFresnel × 0.35 |

---

## 3. Animaciones (Requisito: 3 tipos) — CUMPLE

### Tipo 1 — Básica (transformaciones directas):
- Caída de nieve (SnowSystem.h) — `y -= fallSpeed × dt`
- Rotación globo M5 — `rotate(animT × 360°)`
- Patrulla de fauna (DecoScene.h) — `sin(t × speed + phase)`

### Tipo 2 — Keyframe (LERP):
- 7 módulos con `animT ∈ [0,1]` interpolado en 8 segundos
- Iceberg M1_IZQ: escala 2.0 → 0.3
- Agua M3_IZQ: Y de -0.30 → 1.80
- Árbol M3_DER: escala semilla → árbol
- Todas usan `glm::mix(inicio, fin, animT)`

### Tipo 3 — Procedural (generativa):
- Olas de agua: 3 sinusoidales compuestas en water.vert
- Nieve: deriva aleatoria procedural
- Niebla: función exponencial `exp(-fog²)` en fragment shader
- Humo fábrica: ascenso + expansión procedural

---

## 4. Recorrido Narrativo — CUMPLE

- **Punto inicial**: Vestíbulo (0, 1.7, 2) con pantalla de título
- **Estructura**: 2 corredores (izq: consecuencias, der: soluciones) + sala M5 (acuerdos)
- **7 módulos interactivos** con textos narrativos extensos (datos reales)
- **Punto final**: Pantalla de cierre con mensaje reflexivo + celebración con confeti

---

## 5. Interacciones Contextuales — CUMPLE

| Interacción | Mecanismo |
|-------------|-----------|
| Tecla E | Activa módulo cercano (trigger 5m) |
| Popup narrativo | Texto específico por módulo (7 variantes) |
| Narración de voz | 7 MP3 contextuales (es-ES-AlvaroNeural) |
| Contador progreso | Checklist de 7 módulos visitados |
| Colisiones | Bidireccionales, mantienen dentro del recorrido |

---

## 6. Texturas y Materiales — CUMPLE (95%)

### Texturas:
- `ice.png` (1024×1024) — piso del museo
- `floor.png` (1024×1024) — paredes
- Skybox 6 caras PNG — horizonte ártico
- 24 pinturas enmarcadas en galería (12 únicas × 2 muros)

### Materiales diferenciados:
| Material | Shininess | Fresnel F0 |
|----------|-----------|------------|
| Hielo | 128-256 | 0.04 |
| Metal | 180 | 0.80 |
| Agua | 128 | 0.02-0.03 |
| Concreto | 16 | 0.04 |

### Pendiente menor:
- Modelos Kenney (building_a/b/c, electric_car) usan color override en vez de textura embebida (`colormap.png` no encontrada)

---

## 7. Audio — CUMPLE

- **Ambient**: Viento ártico loop continuo
- **Narración**: 7 MP3 por módulo (generados con Edge TTS)
- **Celebración**: Música al completar los 7 módulos
- **Motor**: miniaudio 0.11+ (header-only, sin DLLs)

---

## 8. UI/HUD — CUMPLE

5 paneles Dear ImGui:
1. Pantalla de título (contexto + instrucciones)
2. Contador de exhibiciones (esquina superior derecha)
3. Panel debug (FPS + cámara, esquina superior izquierda)
4. Panel narrativo inferior (indicador E + barra de progreso)
5. Pantalla de cierre (mensaje reflexivo + progreso)

---

## 9. Fluidez/FPS — CUMPLE

- 165 FPS promedio (VSync off, 1280×720)
- 140+ FPS mínimo (peor caso con todos los módulos activos)
- Optimizaciones: face culling, depth test, fog O(1), skybox al final

---

## 10. Entregables — 80%

| Entregable | Estado |
|------------|--------|
| Ejecutable .exe | Compilado y funcional |
| Reporte español (APA) | Completo en docs/reports/ |
| Reporte inglés | Completo en docs/reports/ |
| Guión de video | Listo en docs/reports/ |
| **Video demostrativo** | **Pendiente — acción del equipo (OBS + DaVinci)** |
| **Instalador InstallForge** | **Pendiente — acción del equipo** |
| **Presentación oral** | **Pendiente — acción del equipo** |

---

## Lo que falta por implementar

### Crítico (afecta calificación):
1. **Instalador InstallForge** — Requerido para entrega. Archivos en `dist/`, falta crear .exe con GUI de InstallForge
2. **Video demostrativo** — 3-5 minutos, guión listo, falta grabar con OBS y editar con DaVinci Resolve
3. **Presentación oral** — Preparar y ensayar

### Opcional (mejora visual):
4. **Fix texturas Kenney** — Re-exportar modelos con textura embebida desde Blender
5. **Fuente Roboto en ImGui** — Cargar TTF para mejor tipografía

### Ya no es necesario:
- Iluminación: excede requisitos (5 de 2 mínimo)
- Environment mapping: excede requisitos (2 de 1 mínimo)
- Animaciones: los 3 tipos están cubiertos
- Audio: completo con narración contextual
- UI/HUD: completo con 5 paneles
