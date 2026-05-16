# Regla: Gestión de Archivos del Proyecto

**Prioridad**: Crítica — aplica a todo trabajo autónomo de Claude en este proyecto.

---

## 1. Confinamiento del proyecto

**Todo archivo creado o modificado para el proyecto debe vivir dentro de `CGEIHC-P/`.**

- El código C++, shaders y assets del proyecto gráfico viven en `CGEIHC-P/app/`
- Los artefactos temporales de build viven en `CGEIHC-P/build/` (gitignored)
- Los screenshots y logs de verificación viven en `CGEIHC-P/active/` (gitignored)
- Los commits de git respaldan todo — si algo no está en `CGEIHC-P/`, no está respaldado

**Nunca crear archivos del proyecto fuera de `CGEIHC-P/`.**

---

## 2. Gestión de descargas

Los assets descargados (modelos 3D, texturas, audio, fuentes) pueden llegar a:
```
C:\Users\herna\Downloads\
```

**Protocolo obligatorio después de cada descarga**:

1. **Verificar** que el archivo descargado es el correcto (formato, calidad, licencia CC0)
2. **Mover** a su subcarpeta correspondiente en `CGEIHC-P/app/assets/`:
   - Modelos 3D (GLTF/OBJ/FBX) → `app/assets/models/`
   - Texturas (PNG/JPG) → `app/assets/textures/`
   - Audio (OGG/WAV) → `app/assets/audio/`
   - Fuentes (TTF) → `app/assets/fonts/`
   - Caras de skybox (PNG) → `app/assets/skybox/`
3. **Eliminar** de `Downloads/` los archivos que ya se movieron o que no se van a usar
4. **Nunca dejar** archivos del proyecto regados en `Downloads/`

---

## 3. Archivos temporales

- Los archivos de `active/` (screenshots, state.json) son temporales — no commitear
- Los archivos de `build/` son generados — no commitear (ya está en .gitignore)
- Si se descarga algo para evaluación y al final no se usa, eliminarlo de `Downloads/`

---

## Referencia rápida

| Tipo de archivo | Ubicación correcta |
|----------------|-------------------|
| Código fuente C++ | `app/src/` |
| Shaders GLSL | `app/shaders/` |
| Headers vendor (stb, GLAD) | `app/src/vendor/` |
| Modelos 3D | `app/assets/models/` |
| Texturas | `app/assets/textures/` |
| Audio | `app/assets/audio/` |
| Fuentes TTF | `app/assets/fonts/` |
| Skybox PNGs | `app/assets/skybox/` |
| Build output | `build/` (gitignored) |
| Screenshots verificación | `active/` (gitignored) |
| Documentación | `docs/` |
| Planos del proyecto | `blueprints/` |
