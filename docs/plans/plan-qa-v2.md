# Plan QA Visual v2 — Pruebas por Comparación de Expectativas

> Fecha: 2026-05-16
> Revisión: Post-auditoría Gemini + GPT
> Referencia: docs/plans/expectativas-visuales.md
> Principio: lo que se VE en pantalla debe coincidir con lo descrito. Si no, es un bug.

---

## Filosofía del QA

> **No verificar "se renderiza", verificar "comunica el mundo diseñado".**

El QA anterior falló porque verificó presencia de píxeles, no significado visual. Este plan corrige eso con:
1. **Multi-ángulo**: cada posición se captura en 3 orientaciones (suelo, frente, techo)
2. **QA semántica**: preguntarse "¿esto parece X?" antes de aceptar
3. **Métricas cuantitativas**: % de pantalla, contraste, cobertura
4. **Bugs conocidos pre-identificados**: winding order del suelo, letreros finos

---

## Bugs Pre-Identificados (corregir ANTES de QA)

| # | Bug | Causa raíz | Archivo:Línea | Fix |
|---|-----|-----------|---------------|-----|
| B1 | Suelo invisible | Winding order CW en vez de CCW → back-face culled | Museum.h:245 | Invertir índices: `{0,2,1, 0,3,2}` |
| B2 | Letreros como líneas diagonales | Cubos de 0.06m grosor vistos de canto | Museum.h:drawSigns() | Engrosar paneles a 0.15m mínimo |
| B3 | Modelos GLB sin textura (grises) | Texturas externas no cargan (path Unicode) | Model.h | Los modelos Kenney usan colores de material — verificar que baseColor se aplica |

---

## Metodología por Punto de Control

```
Para cada PC:

1. PREPARAR
   - Leer expectativa detallada del PC
   - Configurar cámara (posición + yaw + pitch)

2. CAPTURAR MULTI-ÁNGULO (3 fotos por posición)
   - Foto A: pitch=0 (frente/horizonte)
   - Foto B: pitch=-60 (mirando al suelo)
   - Foto C: pitch=+45 (mirando al techo)

3. QA SEMÁNTICA (preguntarse antes de evaluar items)
   - "¿Qué estoy viendo? ¿Reconozco un museo/corredor/módulo?"
   - "¿Hay algo que un usuario diría 'eso está roto'?"
   - "¿El suelo es visible? ¿Las paredes se ven como paredes?"

4. COMPARAR ITEM POR ITEM
   - Cada elemento de la expectativa → ¿presente y correcto?
   - Métricas cuantitativas:
     * Objeto principal debe ocupar >15% del alto de pantalla
     * Suelo debe ser visible (>10% del área inferior de la imagen)
     * Ningún objeto importante <2% de pantalla

5. DIAGNOSTICAR cada diferencia
   - Leer código fuente del componente
   - Usar técnicas de aislamiento si es necesario
   - Hipótesis → verificación → fix

6. CORREGIR + RE-CAPTURAR
   - Aplicar fix
   - Rebuild
   - Re-capturar las 3 fotos
   - Re-evaluar QA semántica + items

7. ITERAR hasta 0 diferencias en ese PC
   - Solo avanzar al siguiente PC cuando este pase al 100%
```

---

## Implementación Técnica del Multi-Ángulo

Para capturar multi-ángulo, modificar el test mode en main.cpp para:
1. Frame 60: pitch=0 (frente) → screenshot "f060_front.png"
2. Frame 120: pitch=-60 (suelo) → screenshot "f120_floor.png"
3. Frame 180: pitch=+45 (techo) → screenshot "f180_ceil.png"
4. Frame 300: pitch=0 + animación en progreso → screenshot "f300_anim.png"
5. Frame 600: pitch=0 + animación avanzada → screenshot "f600_final.png"

Esto da **5 screenshots por PC** en vez de 3 genéricos.

---

## Orden de Ejecución

| Orden | PC | Posición | Descripción | Prioridad |
|-------|-----|----------|-------------|-----------|
| 0 | PRE | — | Corregir bugs B1, B2, B3 antes de empezar | BLOQUEANTE |
| 1 | PC-02 | (0, 1.7, 2.0) | Vestíbulo — primera impresión | CRÍTICA |
| 2 | PC-02b | (0, 1.7, 2.0) yaw=270 | Vestíbulo — mirando hacia atrás (entrada) | ALTA |
| 3 | PC-03 | (-12, 1.7, 9.5) | Corredor izq — M1_IZQ | ALTA |
| 4 | PC-07 | (12, 1.7, 9.5) | Corredor der — M1_DER (simetría) | ALTA |
| 5 | PC-06 | (-12, 1.7, 39.5) | M3_IZQ edificios GLB | ALTA |
| 6 | PC-08 | (12, 1.7, 24.5) | M2_DER auto GLB | ALTA |
| 7 | PC-09 | (12, 1.7, 39.5) | M3_DER árbol GLB | MEDIA |
| 8 | PC-04 | (-12, 1.7, 9.5) activado | M1_IZQ iceberg animando | MEDIA |
| 9 | PC-05 | (-12, 1.7, 24.5) | M2_IZQ oso polar | MEDIA |
| 10 | PC-10 | (0, 1.7, 59.5) | M5 globo | MEDIA |
| 11 | PC-11 | (0, 1.7, 2.0) pitch=0 | Vista exterior (agua + skybox) | MEDIA |

---

## Diagnóstico Avanzado — 8 Técnicas

### T1: Invertir culling
```cpp
glFrontFace(GL_CW); // invertir lo que se considera front-face
```
Si el componente APARECE → winding order está invertido.

### T2: Wireframe mode
```cpp
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
```
Revela geometría real: escalas, gaps, mesh degenerado.

### T3: Forzar color rojo
Si aparece rojo → geometría OK, textura es el problema.

### T4: Desactivar cull face
```cpp
glDisable(GL_CULL_FACE);
```
Si aparece → normales invertidas (se veía como back-face).

### T5: Vista cenital (Y=20, pitch=-90)
Vista de pájaro para verificar posiciones y escalas relativas.

### T6: Escala ×10 temporal
Agrandar un componente invisible para localizarlo.

### T7: Normal visualization (debug shader)
```glsl
FragColor = vec4(Normal * 0.5 + 0.5, 1.0);
```
Colores RGB = dirección de normales. Negro = normales incorrectas.

### T8: Desactivar niebla + partículas
Aislar problemas de textura/iluminación sin efectos atmosféricos.

---

## Criterios de Aceptación por PC

Un PC pasa SOLO si TODO esto se cumple:

### Funcional
- [ ] Cada elemento de la expectativa es visible en la foto FRENTE
- [ ] El suelo es visible en la foto SUELO (>80% del frame)
- [ ] El techo es visible en la foto TECHO (>60% del frame)

### Estético
- [ ] Ningún elemento es completamente negro, magenta, o blanco saturado
- [ ] Las texturas muestran detalle (variación de color, no plano uniforme)
- [ ] La iluminación produce gradientes (superficies no flat)
- [ ] No hay z-fighting, gaps, o triángulos sueltos

### Semántico
- [ ] Un usuario diría "esto es un museo/corredor/sala"
- [ ] Los objetos son reconocibles ("eso es un árbol/auto/edificio")
- [ ] Los letreros son paneles flotantes, no líneas cruzando la pantalla
- [ ] El objeto principal del módulo ocupa >15% del alto de pantalla

### Rendimiento
- [ ] FPS > 55

---

## Registro de Auditoría

### Consulta 02 — Revisión del plan QA v2 (2026-05-16)

**Revisores**: Gemini 2.5 Pro + ChatGPT GPT-4o
**Veredicto**: Plan original no detectaría todos los bugs
**Mejoras incorporadas**:
- Multi-ángulo (pitch -60/0/+45) por cada PC
- QA semántica ("¿esto parece un museo?")
- Bugs pre-identificados (winding order, letreros, texturas)
- 8 técnicas de diagnóstico (vs 4 originales)
- Métricas cuantitativas (% pantalla, cobertura)
- Criterios de aceptación expandidos (funcional + estético + semántico)
