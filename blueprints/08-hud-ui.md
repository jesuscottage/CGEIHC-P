# Doc 08 — HUD/UI

> Estado: COMPLETO
> Última actualización: 2026-05-16

---

## HUD Durante el Recorrido

**HUD completamente vacío** — ningún elemento permanente en pantalla durante el recorrido.
El museo es visualmente limpio: solo el entorno 3D y los letreros informativos en escena.

La única excepción es el indicador `[E] Explorar`, que aparece contexualmente (ver sección siguiente).

---

## Indicador `[E] Explorar`

Visible únicamente cuando el jugador está dentro del trigger de una plataforma:

| Parámetro | Valor |
|-----------|-------|
| Posición | Centrado horizontalmente, 40 px desde el borde inferior de la ventana |
| Fondo | Rectángulo redondeado azul oscuro semi-opaco (#0d2137, alpha ~0.75) |
| Texto | `[E] Explorar` — blanco, Roboto 18 px |
| Implementación | Dear ImGui — `ImGui::SetNextWindowPos` + `ImGuiWindowFlags_NoDecoration` |

---

## Pantalla de Título

Se muestra al abrir la aplicación. Implementada con Dear ImGui + textura de fondo.

### Imagen de fondo

| Parámetro | Valor |
|-----------|-------|
| Contenido | Fotografía del paisaje ártico — oso polar, hielo, aurora boreal, o similares |
| Fuente | Unsplash o Pexels (licencia libre para uso no comercial / académico) — buscar "arctic polar bear" o "arctic landscape" |
| Implementación | Quad fullscreen renderizado antes de ImGui con la textura cargada vía stb_image |
| Oscurecimiento | Quad negro encima con alpha ~0.45 para legibilidad del texto sobre la foto |

### Elementos de texto (Dear ImGui sobre la foto)

| Elemento | Valor |
|----------|-------|
| Título | `"Nuestro Mundo"` — Roboto 48 px, blanco, centrado |
| Subtítulo | `"Calentamiento global en el Polo Norte"` — Roboto 22 px, blanco semi-opaco, centrado |
| Instrucción | `"[Enter] Comenzar"` — Roboto 18 px, blanco, centrado, parte inferior |

### Transición

Fade-out a negro (~0.5 s) → carga del escenario → fade-in al escenario (~0.5 s).
Implementado con un quad negro cuya opacidad se interpola con `glfwGetTime()`.

---

## Pantalla de Cierre

Se activa al presionar Escape cuando el jugador está en M5 con la animación en estado `COMPLETADO`.

### Elementos

| Elemento | Valor |
|----------|-------|
| Fondo | Fade-in a negro desde la escena (~1 s) |
| Frase principal | *"El Ártico no espera. Tú tampoco tienes que hacerlo."* — Roboto 32 px, blanco, centrado |
| Párrafo de cierre | *"Lo que viste aquí no es ficción — es el estado actual de nuestro planeta."* — Roboto 18 px, blanco semi-opaco |
| Créditos | Equipo del proyecto + UNAM Facultad de Ingeniería + CGEIHC 2026-2 — Roboto 14 px, gris claro |
| ODS 13 | `"Este proyecto se alinea con el ODS 13 de la ONU: Acción por el Clima."` — Roboto 14 px |
| Instrucción de salida | `"[Escape] Salir"` — Roboto 16 px, centrado, parte inferior |

Un segundo Escape desde esta pantalla cierra la aplicación.

---

## Tipografía

| Uso | Fuente | Tamaño |
|-----|--------|--------|
| Títulos (pantalla de título / cierre) | Roboto Regular TTF | 48 px / 32 px |
| Subtítulos | Roboto Regular TTF | 22 px |
| Texto general | Roboto Regular TTF | 18 px |
| Créditos / notas pequeñas | Roboto Regular TTF | 14 px |

- Fuente: Google Fonts — Roboto Regular (licencia Apache 2.0, libre para uso académico)
- Se cargan con `ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", size)`
- El archivo TTF va en `assets/fonts/`

---

## Notas de Implementación

- La imagen de fondo de la pantalla de título se carga con stb_image y se sube como textura OpenGL al arranque
- El quad fullscreen de fondo se renderiza con un shader mínimo (sólo textura, sin iluminación) antes de llamar a ImGui
- El oscurecimiento se logra con `glBlendFunc` + quad negro semitransparente encima de la foto
- `ImGui::PushFont` / `ImGui::PopFont` permite cambiar tamaño de fuente dentro de la misma ventana
- Los estados de la app: `TITULO → JUGANDO → CIERRE → SALIR` manejados con un enum en el bucle principal
