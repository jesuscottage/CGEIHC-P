# Pendientes del Proyecto — CGEIHC-P

> Proyecto: Calentamiento global en el polo norte
> Ultima actualizacion: 2026-05-16
> Estado general: **QA Visual v3 — Modelos Quaternius CC0 integrados, escalas calibradas**

---

## Estado Actual

### Modelos 3D (16 archivos GLB en app/assets/models/)

| Modelo | Origen | Textura | Uso en escena |
|--------|--------|---------|---------------|
| pine_snow.glb | Quaternius CC0 | Embebida | M3_DER arbol que crece |
| electric_car.glb | Kenney CC0 | Color override azul | M2_DER auto electrico |
| building_a/b/c.glb | Kenney CC0 | Color override concreto | M3_IZQ ciudad inundada |
| globe.glb | KhronosGroup CC0 | Embebida (metal) | M5 globo giratorio |
| iceberg.glb | Quaternius CC0 | Color override azul hielo | M1_IZQ iceberg derritiendose |
| polar_bear.glb | Quaternius CC0 | Color override crema | M2_IZQ oso polar sobre hielo |
| fox.glb | KhronosGroup CC0 | Embebida | Fauna decorativa (zorro) |
| seagull.glb | three.js (Stork) | Color override blanco | Fauna decorativa (ave) |
| seal.glb | Quaternius CC0 | Color override gris | Fauna decorativa (foca) |
| whale.glb | Quaternius CC0 | Color override azul-gris | Fauna decorativa (ballena) |
| rock.glb | Quaternius CC0 | Color override gris | Decoracion rocas (4 posiciones) |
| tree.glb | Kenney CC0 | Sin textura (legacy) | No usado (reemplazado por pine_snow) |

### Resultados QA Visual v3

| Escena | Estado | Notas |
|--------|--------|-------|
| Vestibulo | Funcional | Suelo texturizado, paredes, techo. Espacio vacio pero funcional |
| M1_IZQ Iceberg | OK | Roca azul que encoge con animT. Escala 5.0 base |
| M2_IZQ Oso polar | OK | Wolf modelo crema sobre disco de hielo |
| M3_IZQ Edificios | OK | 3 edificios concreto a escala 1.8. Agua sube con animT |
| M1_DER Turbina | Funcional | Procedural: poste + gondola + 3 palas rotatorias |
| M2_DER Auto | OK | Kenney car azul con movimiento sinusoidal |
| M3_DER Arbol | OK | Pino nevado que crece de semilla a arbol completo |
| M5 Globo | OK | DamagedHelmet texturizado, rotacion constante |
| Fauna | OK | Zorro, ave, foca, ballena, 4 rocas dispersas |
| Skybox | OK | Cubemap procedural artico con aurora |
| Agua | OK | Shader sinusoidal + Fresnel, Y=-0.6 (no traspasa suelo) |
| Nieve | OK | 100 billboards cayendo |

---

## Bugs Visuales Conocidos

### Criticos (afectan presentacion)
- [ ] **Modelos Kenney sin textura embebida** — tree.glb, electric_car.glb, building_a/b/c.glb referencian `Textures/colormap.png` que no existe; se fuerzan colores override como workaround
- [ ] **Letreros 3D como lineas blancas** — los cubos en cruz de 15cm grosor son visibles como lineas diagonales desde lejos; considerar eliminarlos

### Cosmeticos (no bloquean entrega)
- [ ] **Vestibulo vacio** — no hay elementos decorativos al inicio; primera impresion es de corredor generico
- [ ] **Turbina procedural delgada** — las palas de cubo son dificiles de ver; un modelo GLB seria mejor
- [ ] **Fuente Roboto no cargada** — ImGui usa su fuente default por limitacion de paths Unicode en Windows

---

## Pendientes para Entrega Final

### Por el equipo (accion manual)
- [ ] **Grabar video** con OBS + editar con DaVinci Resolve (guion en docs/reports/guion-video.md)
- [ ] **Crear instalador** con InstallForge GUI desde dist/
- [ ] **Convertir reportes a PDF** con Pandoc o Word
- [ ] **Presentacion oral**

---

## Fases de Implementacion — Todas Completadas

| Fase | Descripcion | Estado |
|------|-------------|--------|
| 0 | Bootstrap CMake + dependencias | Completada |
| 1 | Motor core (Window, Input, Time, CameraFPS) | Completada |
| 2 | Pipeline de render (Shader, Mesh, Texture, MVP) | Completada |
| 3 | Asset pipeline (Model.h con Assimp) | Completada |
| 4 | Layout del museo (suelo, techo, paredes, plataformas) | Completada |
| 5 | Animaciones LERP (7 modulos, tecla E, test mode) | Completada |
| 6 | Blinn-Phong + Fresnel | Completada |
| 7A | Skybox cubemap artico procedural | Completada |
| 7B | Shader de agua (vertex displacement + Fresnel) | Completada |
| 8 | Nieve billboards + fauna decorativa | Completada |
| 9 | UI ImGui + letreros 3D + miniaudio | Completada |
| 10 | Integracion narrativa (titulo/cierre) | Completada |
| 11 | QA rendimiento (59-61 FPS) | Completada |
| Assets | Texturas CC0 + modelos Quaternius/Kenney/KhronosGroup | Completada |
| QA Visual | Verificacion escena por escena con screenshots | Completada |
