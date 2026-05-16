# Doc 02 — Layout del Escenario

> Estado: COMPLETO
> Última actualización: 2026-05-15

---

## Forma General

Planta en **T invertida**: sala de bienvenida central con dos corredores que se abren a izquierda y derecha, ambos convergiendo en el M5 compartido que es el punto final del recorrido.

```
         [VESTÍBULO]  ← posición inicial del jugador
          /       \
    [IZQ]           [DER]
    M1→M2→M3        M1→M2→M3
          \       /
           [ M5 ]  ← final del recorrido
```

---

## Unidad de Escala

| Parámetro | Valor |
|-----------|-------|
| 1 unidad OpenGL | 1 metro |
| Altura del jugador (cámara) | 1.7 unidades |
| Eje Y | vertical (arriba) |

---

## Dimensiones por Zona

| Zona | Dimensiones |
|------|-------------|
| Vestíbulo | 20 × 20 m |
| Cada corredor (izq / der) | 8 m ancho × 60 m largo |
| Sala de cada módulo (M1–M3) | 12 × 12 m (plataforma circular r=5 m dentro) |
| Sala M5 (final compartido) | 20 × 20 m (plataforma circular r=5 m) |

---

## Distribución de Módulos en Cada Corredor

Módulos equidistantes cada 15 m, con plataformas alternando lados del pasillo:

```
Vestíbulo → 15m → [M1] → 15m → [M2] → 15m → [M3] → converge → [M5]
                    izq           der           izq    (alternado)
```

- M1 izq: +15 m desde el vestíbulo
- M2 izq: +30 m
- M3 izq: +45 m → conecta con M5

*(Misma distribución para el corredor derecho, en espejo)*

---

## Terreno

- **Geometría base**: un plano único que cubre toda la escena
- **Variación**: heightmap muy suave via shader — desplazamiento de vértices ±0.3 m máximo
- **Aspecto**: hielo/nieve
- **Pasillos y plataformas**: perfectamente planos (vértices sin desplazar en esas zonas) para movimiento predecible del jugador

---

## Límites del Jugador

- **Mecanismo**: paredes invisibles AABB (cajas de colisión axialmente alineadas)
- **Sin objetos grandes** que simulen muros — el horizonte y el skybox son completamente visibles
- Los modelos de fauna y decoración pueden posicionarse **en cualquier punto de la escena**, incluyendo más allá de los límites del jugador, creando la sensación de un ecosistema vivo y extenso

---

## Entorno Visual

- **Skybox**: cielo ártico (cubemap) — visible desde cualquier punto del museo
- **Sin techo**: espacio semiabierto, el skybox define la atmósfera
- **Sin paredes naturales (glaciares)** como guía — el camino se define exclusivamente por las flechas en el suelo y la distribución de los módulos
- Los modelos decorativos (fauna, elementos árticos) se dispersan libremente por todo el escenario

---

## Señalización y Guía Visual

| Elemento | Descripción |
|----------|-------------|
| Flechas en el suelo | Marcas texturizadas sobre el hielo apuntando hacia cada ruta desde el vestíbulo |
| Cartel de módulo | Billboard o letrero 3D visible desde el corredor con el nombre del módulo |
| Plataforma elevada | Círculo r=5 m elevado 0.3 m, con borde luminoso — indica zona interactiva |
| Panel informativo | Plano/poste dentro de la plataforma con el texto del módulo |

*Nota: la iluminación diferenciada por ruta quedó descartada. La guía es 100% diegética.*

---

## Plataformas de Módulos

- **Forma**: círculo, radio = 5 m
- **Elevación**: 0.3 m sobre el terreno base
- **Borde**: emisivo/luminoso para indicar zona activa
- **Contenido**: modelo principal centrado + panel informativo lateral
- **Trigger**: al entrar en la plataforma se activa la interacción del módulo

---

## Posición Inicial del Jugador

| Parámetro | Valor |
|-----------|-------|
| Posición | `(0, 1.7, 0)` — centro del vestíbulo, altura de ojos |
| Dirección de vista | Norte (hacia los dos caminos) |
| Control | WASD + mouse activos desde el primer fotograma |

---

## M5 — Punto Final del Recorrido

- Zona circular donde convergen ambas rutas
- Plataforma elevada con el modelo central del M5
- Panel con el texto del M5 y el mensaje de cierre del museo
- **Es el último punto del recorrido** — no hay zona final separada

---

## Notas de Implementación

- El plano del terreno puede ser un único mesh de alta subdivisión o generarse proceduralmente
- Las plataformas circulares son segmentos de polígono (32 lados aprox.) — simples de construir
- Las paredes invisibles se implementan como check de posición en el update del jugador:
  `if (player.x > MAX_X) player.x = MAX_X;`
- Toda la escena usa el mismo sistema de coordenadas desde el inicio para evitar reescalados
