# Plan: Construcción de Documentos Blueprint

> Última actualización: 2026-05-15
> Objetivo: Tener los 9 documentos de blueprints/ completamente definidos antes de escribir código.

---

## Estrategia

Los documentos se construyen mediante un diálogo estructurado:
- Por cada decisión, Claude propone una sugerencia razonada
- El equipo confirma o modifica
- Sólo cuando hay consenso se escribe el documento

El **orden importa**: cada documento depende del anterior.

```
Visión/Narrativa → Layout → Modelos → Interacción → Materiales → Animaciones → Audio → HUD → Desarrollo
      01              02       03          04             05           06          07     08       09
```

---

## Progreso

- [x] **Doc 01** — Visión y Narrativa ✅ COMPLETO
  - [x] Lote A: Mensaje central, tono, alcance del contenido (preguntas 1-8)
  - [x] Lote B: Selección de módulos y textos verificados con fuentes primarias (preguntas 9-18)
  - [x] Investigación de fuentes → `FUENTES.md` + `docs/knowledge/technology/fuentes-datos-museo.md`

- [x] **Doc 02** — Layout del Escenario ✅ COMPLETO
  - [x] Lote A: Forma, dimensiones y zonas del museo (preguntas 1-6)
  - [x] Lote B: Posicionamiento, terreno y límites (preguntas 7-12)

- [x] **Doc 03** — Catálogo de Modelos ✅ COMPLETO
  - [x] Lote A: Módulos interactivos — modelos + animación por tecla E (preguntas 1-7)
  - [x] Lote B: Fauna decorativa, paneles, señalización, skybox, indicador (preguntas 8-13)

- [x] **Doc 04** — Interacción y Controles ✅ COMPLETO
  - [x] Lote único: movimiento, cámara, triggers, estados, tecla E, pantalla de título, límites (preguntas 1-8)

- [x] **Doc 05** — Materiales y Texturas ✅ COMPLETO
  - [x] Lote único: suelo, plataformas, atmósfera, agua, letreros, fauna (preguntas 1-6)

- [x] **Doc 06** — Animaciones ✅ COMPLETO
  - [x] Lote único: clasificación por tipo, LERP de módulos, procedurales, reset (preguntas 1-7)

- [x] **Doc 07** — Audio ✅ COMPLETO
  - [x] Lote único: ambient, activación, módulos 3D, formatos, assets (preguntas 1-6)

- [x] **Doc 08** — HUD/UI ✅ COMPLETO
  - [x] Lote único: HUD vacío, indicador E, pantalla de título con foto, pantalla de cierre, tipografía (preguntas 1-5)

- [x] **Doc 09** — Orden de Desarrollo ✅ COMPLETO
  - [x] Lote único: fases (0-11), estructura src/, prerequisitos, riesgos, criterio de entrega (síntesis Gemini+GPT)

---

## Criterios de Completitud

Un documento está **completo** cuando:
1. Todas las preguntas de su lote fueron respondidas
2. El equipo confirmó el contenido
3. El archivo en `blueprints/` tiene la información final escrita

---

## Notas de Diseño

- **Simplicidad ante todo**: si hay dos formas de hacer algo, elegir la más simple de programar
- **Módulos aislados primero**: cada módulo debe funcionar y verse bien solo antes de integrar
- **Assets externos**: priorizar modelos de Sketchfab/Poly Haven/Meshy.ai
- **Escalabilidad**: definir una unidad de escala estándar desde el inicio (1 unidad = 1 metro)
