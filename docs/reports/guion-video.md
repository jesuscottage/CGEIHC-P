# Guión del Video Demostrativo

> Duración objetivo: 3-5 minutos
> Formato: MP4/H.264, 1080p o superior
> Editor recomendado: DaVinci Resolve (gratis)
> Captura de pantalla: OBS Studio (gratis)

---

## Preparación Técnica

1. **Captura**: Usar OBS Studio para grabar la ventana de CGEIHC.exe a 60 FPS, 1080p
2. **Audio**: Grabar narración con micrófono (o agregar en post-producción)
3. **Música de fondo**: Opcional, usar audio CC0 ambiental del proyecto

---

## Estructura del Video

### Escena 1 — Introducción (0:00 - 0:30)

**Pantalla**: Título del proyecto sobre fondo oscuro (slide o captura de pantalla de título)

**Narración**:
> "Nuestro Mundo es un museo virtual interactivo en 3D ambientado en el Polo Norte, desarrollado como proyecto final de Computación Gráfica e Interacción Humano-Computadora. El sistema utiliza OpenGL 3.3 con shaders GLSL para crear una experiencia educativa sobre el calentamiento global."

**Elementos visuales**:
- Logo UNAM / Facultad de Ingeniería
- Nombres del equipo
- Stack técnico: C++ / OpenGL / GLSL

---

### Escena 2 — Pantalla de Título (0:30 - 0:45)

**Pantalla**: Ejecutar la aplicación, mostrar la pantalla de título

**Narración**:
> "Al iniciar, el usuario ve una pantalla de bienvenida. Presionando espacio, ingresa al museo."

**Acción**: Presionar ESPACIO para entrar

---

### Escena 3 — Vista General del Museo (0:45 - 1:15)

**Pantalla**: Recorrer el vestíbulo mostrando la estructura del museo

**Narración**:
> "El museo tiene dos rutas: a la izquierda, las consecuencias del calentamiento global; a la derecha, las soluciones sostenibles. Observen el skybox con cielo nublado ártico, el suelo texturizado y las paredes del museo. El agua del océano Ártico rodea el edificio con un shader de olas sinusoidales y efecto Fresnel."

**Elementos a mostrar**:
- Skybox (cielo nublado)
- Texturas de suelo y paredes
- Plano de agua con olas
- Nieve cayendo (billboards)
- Fauna decorativa (foca, zorro, gaviota)

---

### Escena 4 — Ruta Izquierda: Consecuencias (1:15 - 2:15)

**Narración**: Caminar por el corredor izquierdo, activando cada módulo

**M1_IZQ — Deshielo del Iceberg** (1:15 - 1:35):
> "En el primer módulo, un iceberg se derrite progresivamente con la animación keyframe. Al presionar E, la animación comienza: el bloque de hielo reduce su tamaño gradualmente, simulando el deshielo acelerado."

**M2_IZQ — Pérdida de Hábitat** (1:35 - 1:50):
> "El segundo módulo muestra un oso polar sobre una capa de hielo que encoge. A medida que avanza la animación, el hábitat del oso desaparece."

**M3_IZQ — Inundación de Ciudades** (1:50 - 2:15):
> "El tercer módulo presenta edificios de una ciudad siendo inundados. Los modelos de edificios son assets reales en formato GLB cargados con Assimp. El nivel del agua sube progresivamente hasta casi cubrir las estructuras."

---

### Escena 5 — Ruta Derecha: Soluciones (2:15 - 3:00)

**Narración**: Recorrer el corredor derecho

**M1_DER — Energía Eólica** (2:15 - 2:30):
> "La primera solución muestra una turbina eólica que comienza a girar. La velocidad de las aspas aumenta con la animación, representando la adopción de energías renovables."

**M2_DER — Transporte Sostenible** (2:30 - 2:45):
> "Un auto eléctrico en movimiento, renderizado como modelo 3D real de Kenney Assets. Se desplaza con movimiento sinusoidal mientras su color cambia a azul eléctrico."

**M3_DER — Reforestación** (2:45 - 3:00):
> "Un árbol crece desde una semilla hasta su tamaño completo, usando un modelo 3D de cono low-poly. Esta animación keyframe representa la importancia de la reforestación."

---

### Escena 6 — Módulo Compartido M5 (3:00 - 3:30)

**Pantalla**: Navegar a la sala final

**Narración**:
> "Ambas rutas convergen en el módulo final: un globo terráqueo giratorio que representa los acuerdos internacionales contra el cambio climático. Las líneas de conexión aparecen secuencialmente representando la cooperación global."

**Al completar M5**:
> "Al terminar la animación del globo, se activa la pantalla de cierre con un mensaje de reflexión sobre nuestra responsabilidad ambiental."

---

### Escena 7 — Aspectos Técnicos (3:30 - 4:15)

**Pantalla**: Mostrar el panel de debug (FPS, posición) y recorrer escenas

**Narración**:
> "Desde el punto de vista técnico, el sistema implementa iluminación Blinn-Phong con reflexión Fresnel, un skybox cubemap cargado desde archivos PNG, shader de agua con desplazamiento de vértices en tres capas, y un sistema de partículas de nieve con billboards orientados a la cámara. El rendimiento se mantiene estable a 60 FPS en hardware gráfico integrado AMD Radeon."

**Elementos a destacar**:
- Panel debug mostrando FPS estables
- Reflexiones Fresnel en el agua (ángulo rasante)
- Nieve billboard (cambiar ángulo para mostrar orientación)
- Texturas PBR en superficies

---

### Escena 8 — Cierre (4:15 - 4:30)

**Pantalla**: Slide final con créditos

**Narración**:
> "Nuestro Mundo fue desarrollado por el equipo conformado por Jesús Hernández, Juan Pablo Navarro, Carmen Violeta Macias e Israel Martínez. Proyecto final de CGEIHC, Facultad de Ingeniería, UNAM, semestre 2026-2."

---

## Notas de Producción

- **Resolución mínima**: 1280x720 (la app se ejecuta en esta resolución)
- **Framerate de grabación**: 60 FPS
- **Transiciones**: Cortes simples o fade entre escenas
- **Texto en pantalla**: Agregar títulos de cada módulo y datos técnicos como overlays en DaVinci Resolve
- **Duración de cada módulo**: Mostrar la animación desde t=0 hasta t~0.7 (no es necesario esperar a que termine por completo)
