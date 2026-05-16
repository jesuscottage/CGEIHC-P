# Doc 04 — Interacción y Controles

> Estado: COMPLETO
> Última actualización: 2026-05-15

---

## Movimiento del Jugador

| Tecla | Acción |
|-------|--------|
| W | Avanzar |
| S | Retroceder |
| A | Desplazarse a la izquierda (strafe) |
| D | Desplazarse a la derecha (strafe) |
| Mouse | Rotar cámara (pitch + yaw) |

- **Velocidad**: 4 m/s constante — sin correr, sin saltar
- El terreno es completamente plano en los corredores, sin necesidad de física vertical

---

## Sistema de Cámara

| Parámetro | Valor |
|-----------|-------|
| Tipo | FPS — primera persona, sin cuerpo visible |
| Altura de la cámara | y = 1.7 m (fija, nivel de ojos) |
| Pitch | Limitado a ±80° (evita voltear la cámara) |
| Yaw | Libre, 360° |
| Sensibilidad de mouse | Constante configurable en código (`MOUSE_SENSITIVITY`) |
| FOV | 60° (estándar FPS interior) |

---

## Captura del Cursor

| Evento | Comportamiento |
|--------|---------------|
| Inicio de la app | Cursor capturado (`GLFW_CURSOR_DISABLED`) |
| Tecla Escape | Cursor liberado — app en pausa o cierre |
| App pierde foco | Cursor liberado automáticamente por GLFW |

---

## Trigger de Proximidad

La detección de zona activa se calcula por distancia euclidiana en XZ:

```cpp
float dist = length(vec2(player.x - modulo.x, player.z - modulo.z));
bool enZona = (dist < RADIO_TRIGGER);  // RADIO_TRIGGER = 5.0f
```

- **Un solo módulo activo a la vez** — si hay solapamiento, gana el más cercano
- Sin colisión física con la plataforma — el jugador puede caminar sobre ella libremente

---

## Estados de Cada Módulo

```cpp
enum EstadoModulo { INACTIVO, ANIMANDO, COMPLETADO };
```

| Estado | Condición | Comportamiento visible |
|--------|-----------|----------------------|
| `INACTIVO` | Jugador fuera del trigger | Letrero siempre visible, animación en estado inicial |
| `ANIMANDO` | Jugador presionó E | Animación corre hacia adelante, `[E] Explorar` desaparece |
| `COMPLETADO` | Animación llegó al final | Módulo en estado final, letrero sigue visible |

**Al salir del trigger**: el módulo vuelve a `INACTIVO` y la animación hace reset instantáneo al estado inicial.

---

## Tecla E — Activación de Animación

- La tecla E sólo tiene efecto cuando el jugador está dentro del trigger de un módulo
- Activa la animación lerp del módulo (ver Doc 03 para detalles de cada animación)
- El **letrero informativo 3D** es siempre visible — E no controla el texto, sólo la animación
- E no se puede presionar dos veces seguidas para el mismo módulo (se ignora si ya está `ANIMANDO` o `COMPLETADO`)

---

## Pantalla de Título

Pantalla simple implementada con Dear ImGui en modo fullscreen antes de entrar al museo:

| Elemento | Descripción |
|----------|-------------|
| Fondo | Color sólido azul oscuro polar (`#0a1a2e`) |
| Título | `"Nuestro Mundo"` — tipografía grande, centrada |
| Subtítulo | `"Calentamiento global en el Polo Norte"` |
| Instrucción | `"[Enter] Comenzar"` — centrado en la parte inferior |
| Transición | Fade-out a negro → fade-in al escenario (opacidad interpolada en ~1 s) |

La pantalla de título no carga el escenario 3D en segundo plano — carga al presionar Enter para mantener la implementación simple.

---

## Límites del Jugador

Implementados como clamp de posición en el `update()` del jugador, sin señal visual:

```cpp
player.x = clamp(player.x, LIMIT_MIN_X, LIMIT_MAX_X);
player.z = clamp(player.z, LIMIT_MIN_Z, LIMIT_MAX_Z);
```

Los valores de límite corresponden al contorno del museo (vestíbulo + dos corredores + M5). El jugador simplemente se detiene — sin rebote ni efecto visual.

---

## Resumen de Teclas

| Tecla | Función |
|-------|---------|
| W / A / S / D | Movimiento |
| Mouse | Rotar cámara |
| E | Activar animación del módulo actual |
| Enter | Comenzar (sólo en pantalla de título) |
| Escape | Dentro de M5 con animación completada: mostrar pantalla de cierre. En cualquier otro momento: liberar cursor / cerrar app |

---

## Notas de Implementación

- La lógica de movimiento usa `deltaTime` para velocidad independiente del framerate
- El cálculo del trigger se ejecuta cada frame en el `update()` principal
- Los estados de módulo se almacenan en un arreglo: `EstadoModulo estados[NUM_MODULOS]`
- La pantalla de título usa `ImGui::SetNextWindowSize(ImVec2(screenW, screenH))` + `ImGuiWindowFlags_NoDecoration` para ocupar toda la pantalla
- El fade se implementa con `glClearColor` ajustando el alpha, o con un quad negro encima de la escena con opacidad interpolada
