---
title: Idea, Boceto y Storyboard — Calentamiento Global en el Polo Norte
date: 2026-05-06
category: technology
tags: [idea, boceto, storyboard, diseño, narrativa, museo-virtual, opengl]
status: complete
---

# Idea, Boceto y Storyboard

**Proyecto**: Calentamiento global en el polo norte
**Entrega**: 06/05/2026 — Primera parte del proyecto

---

## Idea Central

Experiencia interactiva 3D de un **museo virtual educativo** ambientado en el Polo Norte. El usuario recorre módulos relacionados con el calentamiento global y sus consecuencias. Estructura de exposición guiada e inmersiva que educa mediante exploración visual e interacción directa.

---

## Estructura Narrativa (Boceto)

```
                          [ENTRADA]
                          Bienvenida
                             |
              +--------------+--------------+
              |                             |
    [CAMINO IZQUIERDO]             [CAMINO DERECHO]
    Consecuencias del              Soluciones sostenibles
    calentamiento global
    M1: Disminución hielo          M1: Energías renovables
    M2: Aumento nivel del mar      M2: Transporte sostenible
    M3: Pérdida de diversidad      M3: Construcción sostenible
    M4: Liberación de metano       M4: Captura de carbono
              |                             |
              +--------+   +----------------+
                       |   |
                  [M5 COMPARTIDO]
                  Acuerdos sociales
                       |
                 [ZONA FINAL]
                 Reflexión y conclusión
```

---

## Escenas del Storyboard

### Escena 1: Introducción y Bienvenida

- Cartel de bienvenida y propósito de la simulación
- Información general del calentamiento global y deshielo del Polo Norte
- Entorno polar amplio y tranquilo (ecosistema intacto)
- Dos caminos visibles: ruta izquierda y ruta derecha
- El usuario elige libremente cuál recorrer primero

### Escena 2: Camino Izquierdo — Explicación Científica

**Inicio (ecosistema intacto)**:
- Grandes glaciares, icebergs intactos
- Animales en hábitats estables
- Superficies oceánicas congeladas

**Progresión (degradación gradual)**:
- Glaciares reduciéndose de tamaño
- Grietas en el hielo
- Océano con más movimiento
- Zonas con hielo parcialmente derretido

**Cada módulo incluye**:
- Estadísticas reales
- Paneles informativos
- Animaciones explicativas
- Indicadores climáticos

**Efectos visuales**:
- Partículas de nieve
- Reflejos dinámicos
- Neblina
- Ondas en el océano

### Escena 3: Camino Derecho — Contradicciones y Problemática Humana

**Ambiente visual**: Pesado, deteriorado, urgente

**Contenido**:
- Hielo fracturado, ambientes contaminados
- Humo y residuos industriales
- Agua más oscura y agresiva

**Mensajes**:
- Desinformación climática
- Negación del calentamiento global
- Sobreexplotación de recursos
- Contaminación industrial

**Comparaciones visuales**:
- Ecosistema antes vs. después del deterioro
- Impacto de la actividad humana

### Escena 4: Unión de Caminos — Reflexión Final

- Entorno ártico gravemente deteriorado
- Menor presencia de fauna
- Grandes zonas sin hielo, océanos dominando
- Mensaje sobre responsabilidad ambiental y ODS 13 (Acción por el Clima)
- Estadísticas finales y frases de concientización

---

## Modelos 3D Necesarios

### Entorno Principal
- Terreno nevado
- Icebergs de diferentes tamaños
- Cuevas de hielo
- Glaciares
- Rocas nevadas
- Superficie oceánica
- Skybox polar

### Ruta Problemática
- Hielo fracturado / glaciares derretidos
- Objetos contaminantes, barriles industriales
- Plataformas deterioradas
- Superficies parcialmente inundadas

### Fauna (con rigging)
- Oso polar
- Morsa
- Foca
- Pingüinos
- Caribúes
- Zorro ártico
- Peces y aves árticas
- (Posible personaje guía)

### Ruta Sostenible
- Paneles solares
- Turbinas eólicas
- Casas ecológicas
- Vehículos eléctricos
- Estaciones tecnológicas
- Sistemas de reciclaje

### Elementos Interactivos
- Carteles informativos
- Señalamientos y flechas de recorrido
- Boyas científicas
- Indicadores de temperatura
- Paneles holográficos

---

## Animaciones Planificadas

### Procedurales (automáticas, sin keyframes)
- Movimiento del océano (Perlin Noise, funciones senoidales, desplazamiento de vértices)
- Partículas de nieve (vectores aleatorios controlados matemáticamente)
- Niebla variable por zona
- Humo y gases
- Flujo de energía

### Por Keyframes (LERP/SLERP)
- Transformación de glaciares (intacto → derretido)
- Apertura de módulos interactivos
- Movimiento de puertas
- Activación de paneles educativos

### Fauna (rigging + skinning)
- Caminata del oso polar
- Movimiento de focas y morsas
- Vuelo de aves árticas
- Peces nadando

### Ruta Problemática
- Fractura de glaciares
- Colapso de hielo
- Inundaciones graduales
- Liberación de partículas de metano

### Ruta Sostenible
- Turbinas eólicas girando
- Paneles solares funcionando
- Flujo de energía limpia
- Transporte eléctrico en movimiento

---

## Materiales y Texturas

### Ecosistema Polar
- **Hielo**: Transparencia parcial, reflejos especulares, grietas, variaciones de opacidad
- **Agua oceánica**: Alta reflectividad, movimiento dinámico, color variable por profundidad, environment mapping
- Nieve, rocas congeladas, superficies húmedas, cuevas de hielo

### Narrativos por Ruta
- **Ruta problemática**: Materiales oscuros, superficies deterioradas, texturas contaminadas
- **Ruta sostenible**: Materiales limpios, superficies metálicas pulidas, colores claros y optimistas

### Técnicas de Texturizado
- Texturas difusas, normal maps, specular maps
- Reflection maps, environment mapping
- Materiales PBR simplificados
- Shaders GLSL dinámicos (color variable con temperatura, degradación progresiva)

---

## Sistema de Iluminación

**Modelo**: Blinn-Phong
- Luz ambiental
- Luz difusa
- Luz especular

**Por ruta**:
- **Problemática**: Iluminación oscura, tonos azulados/grisáceos, sombras fuertes
- **Sostenible**: Iluminación brillante, colores claros y optimistas, reflejos suaves

---

## Sistema de Cámara

- Tipo: **Free-fly** (vuelo libre)
- Controles: teclado (WASD/flechas) + mouse (orientación) + scroll (zoom)
- Movimiento independiente de FPS via **delta time**
- Cámara narrativa automática: enfoque automático en eventos clave

---

## Sistema de Interacción

- **Triggers por proximidad**: Al acercarse a módulos, se activan animaciones, paneles y sonidos
- **HUD/UI minimalista**: Información climática, temperatura, año simulado, indicadores de recorrido
- **Mapa pequeño** del museo (opcional)

---

## Sonido

- **Ambiente ártico**: Viento helado, crujidos de hielo, océano, fauna
- **Ruta problemática**: Sonidos tensos, hielo quebrándose, industriales
- **Ruta sostenible**: Música tranquila, tecnológica, positiva
- **Interactivos**: Efectos al abrir módulos, activar paneles, iniciar simulaciones

---

## Efectos Visuales Avanzados

- Bloom
- Niebla volumétrica
- Sistema de partículas (nieve, hielo fracturado, humo, metano)
- Profundidad de campo
- Corrección de color
