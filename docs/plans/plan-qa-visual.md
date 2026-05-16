# Plan QA Visual — Verificación Exhaustiva del Proyecto

> Proyecto: Calentamiento global en el polo norte
> Fecha: 2026-05-16
> Ejecutor: Claude Code (autónomo)
> Objetivo: Verificar visualmente que el proyecto está bonito, coherente y listo para presentar

---

## Metodología

Para cada test:
1. Ejecutar `CGEIHC.exe --test-module {MOD} --activate-frame 60`
2. La app captura screenshots automáticos en frames 60, 300, 600
3. Leer cada screenshot con Read (Claude puede ver imágenes)
4. Evaluar contra el checklist visual
5. Si hay defecto → corregir en código → re-ejecutar → re-verificar
6. Si OK → marcar como aprobado

**Screenshots**: 3 por módulo (f060 = pre-animación, f300 = mitad, f600 = ~final)
**Total de módulos**: 7 + 1 vestíbulo = 8 ejecuciones
**Total de screenshots**: ~24 imágenes a evaluar

---

## 1. Entorno General (verificar en TODOS los módulos)

### 1.1 Skybox
- [ ] Las 6 caras del cubemap se ven (cielo nublado, sin bandas negras)
- [ ] No hay costuras visibles entre caras
- [ ] El horizonte es coherente al girar la cámara
- [ ] El skybox está detrás de toda la geometría (depth trick funciona)

### 1.2 Texturas del Museo
- [ ] El suelo muestra textura de concreto (no color plano blanco-azul)
- [ ] Las paredes muestran textura de concreto claro (no color plano)
- [ ] Las plataformas muestran textura de hielo (no color plano azul)
- [ ] Las texturas tienen tiling correcto (no estiradas ni pixeladas)
- [ ] No hay texturas invertidas o rotadas incorrectamente

### 1.3 Iluminación
- [ ] La luz hemisférica es visible (parte superior más clara, inferior más oscura)
- [ ] El componente especular Blinn-Phong produce highlights en superficies lisas
- [ ] El efecto Fresnel es visible en los bordes de objetos (más reflectante a ángulos rasantes)
- [ ] La niebla se ve a distancia (objetos lejanos se desvanecen en el color de niebla)
- [ ] No hay superficies completamente negras o completamente blancas saturadas

### 1.4 Agua
- [ ] El plano de agua es visible alrededor del museo
- [ ] Las olas se mueven (diferente patrón en f060 vs f300 vs f600)
- [ ] El color del agua cambia entre profundo (azul oscuro) y superficial (turquesa)
- [ ] La espuma aparece en las crestas de las olas
- [ ] El efecto Fresnel hace que el agua sea más reflectante a ángulo rasante
- [ ] La transparencia del agua es visible (alpha < 1.0)

### 1.5 Nieve
- [ ] Se ven partículas blancas cayendo
- [ ] Los billboards están orientados hacia la cámara (no se ven de canto)
- [ ] La distribución de nieve es razonablemente uniforme en el espacio visible
- [ ] La nieve cae constantemente (visible en múltiples frames)

### 1.6 UI (ImGui)
- [ ] Panel debug (esquina superior izquierda) muestra FPS y posición
- [ ] Panel narrativo (parte inferior central) muestra nombre del módulo + barra de progreso
- [ ] Los textos son legibles (no cortados, no fuera de pantalla)
- [ ] La barra de progreso avanza de 0 a 1 durante la animación

---

## 2. Módulos Individuales — Checklist Visual

### 2.1 M1_IZQ — Iceberg derritiéndose
- [ ] El iceberg es visible al inicio (bloque grande azul-blanco)
- [ ] La animación reduce progresivamente el tamaño del iceberg
- [ ] El color cambia de blanco-azul a azul más oscuro conforme se derrite
- [ ] El trozo lateral flotante desaparece a mitad de la animación
- [ ] La plataforma circular con textura de hielo es visible debajo
- [ ] El letrero 3D de color es visible sobre el módulo

### 2.2 M2_IZQ — Oso polar en capa de hielo
- [ ] Se ve el disco de hielo (grande al inicio)
- [ ] El oso polar (cubos blancos) está parado sobre el hielo
- [ ] La capa de hielo encoge progresivamente
- [ ] El oso sigue visible incluso cuando la capa es pequeña
- [ ] La fauna cercana (foca) es visible en las inmediaciones

### 2.3 M3_IZQ — Inundación de ciudades
- [ ] Los 3 edificios son modelos GLB reales (no cubos planos)
- [ ] Los edificios tienen colores/materiales del modelo Kenney
- [ ] El agua azul sube progresivamente cubriendo los edificios
- [ ] A animT=1 el agua casi cubre los edificios
- [ ] Los edificios tienen escala coherente con la escena

### 2.4 M1_DER — Turbina eólica
- [ ] El poste vertical es visible (gris metálico)
- [ ] La góndola está en la parte superior
- [ ] Las 3 palas giran (visible comparando f060 con f300)
- [ ] La velocidad de giro aumenta con la animación
- [ ] La turbina tiene escala coherente (~5m de alto)

### 2.5 M2_DER — Auto eléctrico
- [ ] El auto es un modelo GLB real (no cubos planos)
- [ ] El auto tiene forma reconocible de vehículo
- [ ] El auto se mueve sinusoidalmente cuando la animación está activa
- [ ] La escala del auto es razonable (~2m de largo, no gigante ni microscópico)

### 2.6 M3_DER — Árbol que crece
- [ ] El árbol es un modelo GLB real (forma de cono/pino)
- [ ] Al inicio (f060) el árbol es muy pequeño (semilla)
- [ ] A mitad (f300) el árbol ha crecido parcialmente
- [ ] Al final (f600) el árbol tiene su tamaño completo
- [ ] El color verde y marrón del tronco son visibles
- [ ] La escala final es coherente (~3-4m de alto)

### 2.7 M5 — Globo terráqueo
- [ ] El globo es una esfera 3D (modelo GLB)
- [ ] El globo gira constantemente
- [ ] Las líneas de acuerdos (discos) aparecen secuencialmente con animT
- [ ] El globo está elevado (~3m sobre el suelo)
- [ ] La escala es coherente con la sala M5

---

## 3. Fauna Decorativa

- [ ] La foca es visible cerca de M2_IZQ (cuerpo gris alargado + cabeza)
- [ ] El zorro ártico es visible en el corredor izquierdo (cuerpo blanco + orejas)
- [ ] La gaviota está en el aire cerca de M2_IZQ (alas extendidas + pico amarillo)
- [ ] Los animales tienen colores coherentes (no negros ni magenta)
- [ ] La escala es razonable (foca ~1m, zorro ~0.5m, gaviota ~0.5m envergadura)

---

## 4. Letreros 3D

- [ ] Cada módulo tiene un letrero de color flotando sobre él (~5m de altura)
- [ ] Los letreros son paneles en cruz (visibles desde cualquier ángulo)
- [ ] Los colores corresponden a cada módulo (diferenciables entre sí)
- [ ] Los letreros no se superponen con la geometría del techo

---

## 5. Coherencia Visual General

- [ ] La paleta de colores es fría/ártica (azules, blancos, grises)
- [ ] No hay z-fighting visible (superficies parpadeantes)
- [ ] No hay artefactos gráficos (triángulos sueltos, polígonos rotos)
- [ ] La niebla unifica la escena (transición suave a la distancia)
- [ ] El agua, la nieve y el skybox crean una atmósfera polar coherente
- [ ] Los módulos de la ruta izquierda transmiten degradación/urgencia
- [ ] Los módulos de la ruta derecha transmiten esperanza/soluciones
- [ ] El tamaño relativo de los objetos es lógico (edificios > autos > animales)

---

## 6. Rendimiento

- [ ] Todos los módulos > 30 FPS (mínimo aceptable)
- [ ] Todos los módulos > 55 FPS (objetivo de calidad)
- [ ] No hay stuttering visible (caídas bruscas de frame rate)
- [ ] La carga de modelos no causa lag perceptible al inicio

---

## Protocolo de Corrección

Si se detecta un defecto visual:
1. Clasificar severidad: **Crítico** (rompe la presentación) / **Mayor** (se nota) / **Menor** (cosmético)
2. Identificar la causa (shader, escala, posición, textura, color)
3. Corregir en el código fuente
4. Rebuild + re-ejecutar el test del módulo afectado
5. Re-verificar con screenshot
6. Máximo 3 iteraciones por defecto antes de documentar como known issue
