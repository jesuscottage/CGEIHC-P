# Consulta 01 — Auditoría del Plan Maestro de Desarrollo

**Auditor:** Gemini Antigravity (Arquitecto Senior de Software C++/OpenGL)

A continuación presento la auditoría del plan maestro de desarrollo (`docs/plans/plan-maestro-desarrollo.md`), enfocada en su viabilidad para ser ejecutado autónomamente por Claude Code, respetando el stack técnico y las limitaciones académicas.

---

### 1. Orden de fases — ¿Es óptimo?

**Veredicto**: Correcciones menores

**Análisis**: 
El flujo incremental (Bootstrap → Core → Render → Assets → Escenario) minimiza retrabajos al no requerir refactorizaciones grandes en etapas tempranas. Sin embargo, presenta dos problemas logísticos y de carga:
1. **Fase 7 Sobrecargada**: Combinar Agua procedural, Fresnel, Skybox, Niebla y el **orden de renderizado con transparencias (`glDepthMask`)** en una sola fase generará problemas graves de z-fighting y ordenamiento visual. Depurar todo a la vez complicará el ciclo autónomo.
2. **Dear ImGui muy tardío (Fase 9)**: ImGui es implementado casi al final. En el desarrollo asistido por IA, tener un overlay de debug (para ver coordenadas XZ, estados de los módulos o el tiempo `t` de animación) desde el principio ahorra muchísimas iteraciones inútiles y deducciones visuales.

**Correcciones sugeridas**:
- **Dividir la Fase 7**: Separar en Fase 7A (Skybox, Niebla, y definición del pipeline con transparencias) y Fase 7B (Shaders procedimentales: Agua y Fresnel).
- **Adelantar ImGui Core a la Fase 1 o 2**: Implementar la inicialización de Dear ImGui tempranamente exclusivamente para una ventana de debug en pantalla. Dejar el HUD final y menús estilizados para la Fase 9.

### 2. Pipeline de verificación visual autónoma — ¿Es robusto?

**Veredicto**: Correcciones mayores

**Análisis**: 
Usar `glReadPixels` con `stb_image_write` es la mejor decisión técnica para omitir capturas de SO, pero la implementación propuesta (`AUTO_SCREENSHOT_FRAME = 120` y cierre inmediato) es insuficiente para probar interactividad y animaciones LERP de hasta 12 segundos (como el M3 izq).
Si el motor se cierra a los 2 segundos (frame 120 a 60fps), el agente jamás podrá ver ni verificar visualmente si una animación completó su recorrido o si un módulo llegó al estado `COMPLETADO`.

**Correcciones sugeridas**:
- **Soporte CLI para screenshots programados**: Añadir argumentos a `main.cpp` para definir el frame de captura dinámicamente, ej: `./CGEIHC.exe --screenshot-frame 600` o `--screenshot-frames 1,120,600`.
- **Logs de estado por consola**: El pipeline debe imprimir el estado de las variables (posición de cámara, estado del módulo `t`, FPS) cada segundo por `std::cout`. Esto provee telemetría crucial a Claude sin depender exclusivamente del PNG estático.

### 3. Tareas faltantes o en la fase incorrecta

**Veredicto**: Correcciones menores

**Análisis**: 
El diseño está fuertemente alineado con las especificaciones y no hay lagunas graves en el stack (C++/OpenGL 3.3). 
No obstante, se han omitido consideraciones operativas menores pero que a un agente autónomo le causan bloqueos técnicos (hallucinaciones).

**Correcciones sugeridas**:
- **Resolución del Working Directory (CWD)**: Falta una tarea explícita en Fase 0/1 para estandarizar el directorio de trabajo. Si CMake/Claude ejecuta `.\build\Debug\CGEIHC.exe` desde la carpeta build o desde la raíz, las rutas relativas (`assets/...`) fallarán.
- **Clarificación de la numeración**: En la Fase 4, especificar claramente a Claude mediante un comentario en el prompt que "El Módulo 4 (M4) NO EXISTE intencionalmente" para evitar que el agente intente crear tareas o corregir código para suplir un número faltante.

### 4. Criterios de salida — ¿Son verificables autónomamente?

**Veredicto**: Correcciones mayores

**Análisis**: 
Los criterios de salida de las fases estáticas (Fases 0, 1, 2) son perfectos. Sin embargo, los criterios de las Fases 4, 5, y 10 requieren acciones humanas explícitas: *"Jugador recorre el museo"*, *"Los 7 módulos responden a la tecla E"*. 
Claude, corriendo procesos en background en Windows, no puede emitir eventos de teclado WASD nativos ni mover el mouse para verificar estas fases.

**Correcciones sugeridas**:
- **Implementar Telemetría o "Test Mode" en CLI**: Añadir argumentos por línea de comandos para inicializar la app en un estado específico. Por ejemplo, `./CGEIHC.exe --test-module M1_IZQ` que coloque la cámara instantáneamente en `(-12, 1.7, 13)` simulando la presión de 'E'.
- Sin este modo de inyección, la validación de las Fases 5 y 10 requerirá intervención humana obligatoria y bloqueará el desarrollo autónomo.

### 5. Gestión de riesgos — ¿Hay riesgos no cubiertos?

**Veredicto**: Correcciones mayores

**Análisis**: 
Los riesgos definidos (Assimp, ImGui, FetchContent, stb_truetype, y orden de transparencia) están excelentemente analizados y las mitigaciones son sólidas para este stack. Sin embargo, faltan los riesgos derivados del propio **paradigma de IA autónoma**.

**Correcciones sugeridas** (Añadir a la tabla de riesgos):
- **Riesgo 6: Falla de carga por Working Directory**. (Mitigación: Usar variables de CMake para copiar la carpeta `assets/` a la ruta del binario post-build, o establecer una función base path en C++).
- **Riesgo 7: Claude no puede generar inputs en runtime**. (Mitigación: Los argumentos de test CLI mencionados arriba para forzar el trigger y simular la tecla E).

---

## Resumen Ejecutivo

**Veredicto Global: NECESITA AJUSTES MENORES (Enfoque en Autonomía)**  
El plan arquitectónico es excepcionalmente sólido para un proyecto C++17/OpenGL académico, y el stack propuesto (stb, miniaudio, fetchcontent) es elegante y minimalista. Sin embargo, para que **Claude Code pueda ejecutarlo de manera verdaderamente autónoma**, las barreras operativas deben mitigarse.

**Top 4 Correcciones Más Importantes (Por Impacto):**
1. **Añadir "Test Mode" por CLI**: Fundamental. Claude debe poder teletransportar la cámara y simular la pulsación de 'E' mediante un comando de inicialización (`--test-module`) para poder tomar screenshots de las Fases 5 y 10.
2. **Modificar captura de Screenshot**: Permitir que el cierre de la ventana y la captura (`AUTO_SCREENSHOT_FRAME`) se ajusten por CLI, para capturar animaciones que toman 5-10 segundos, en lugar de cerrar a los 2 segundos siempre.
3. **Estabilizar las rutas relativas (Assets)**: Implementar un paso en CMake (`add_custom_command POST_BUILD`) que copie la carpeta de assets junto al `.exe`, o fijar el `std::filesystem::current_path()`.
4. **Desacoplar la Fase 7 y adelantar ImGui (Debug)**: Evitar agrupar tantas técnicas complejas de shader en una sola fase e inicializar Dear ImGui temprano para imprimir el estado del juego en pantalla (esto ayudará al agente a entender sus propios screenshots).
