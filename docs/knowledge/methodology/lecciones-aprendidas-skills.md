---
title: Lecciones aprendidas — mejoras generales para skills jurídicas
date: 2026-04-25
category: methodology
tags: [skills, mejora-continua, errores-recurrentes, calidad]
status: vivo
---

# Lecciones aprendidas — skills jurídicas

> **Documento vivo**. Se carga **siempre** al inicio de toda skill jurídica
> y se alimenta tras cada ejecución con patrones generales que valga la
> pena no volver a tropezar.

---

## Propósito

Capturar, en un solo lugar y de forma estructurada, los aprendizajes
acumulados al usar las skills jurídicas reales. Cada nueva ejecución
debe partir de un piso más alto que la anterior.

## Reglas de contenido (estrictas)

| ✅ SÍ va aquí | ❌ NO va aquí |
|---|---|
| Sesgos del modelo (confusiones recurrentes entre términos jurídicos parecidos) | Correcciones específicas a un documento de un caso concreto |
| Ambigüedades de instrucciones que se repiten ("actualiza la fecha" — ¿cuál?) | Datos de clientes (nombres, RFC, fechas, montos, direcciones) |
| Convenciones del abogado validadas tras varias iteraciones | Preferencias estilísticas para un único documento |
| Limitaciones técnicas con workaround conocido (p. ej. resolución mínima para OCR) | Ajustes puntuales a un caso particular |
| Errores estructurales generales en plantillas (p. ej. cláusulas heredadas obsoletas) | Errores que sólo ocurrieron una vez sin patrón evidente |

> **Si una entrada contiene un nombre, una fecha específica o un dato del
> caso, reformúlala en términos generales o no la registres.**

## Formato de entrada

```markdown
### [YYYY-MM-DD] Título corto del patrón

- **Qué pasaba**: descripción del error o fricción observada.
- **Por qué**: causa raíz (sesgo del modelo, ambigüedad, limitación
  técnica, convención del abogado, etc.).
- **Cómo evitarlo**: regla concreta a aplicar en futuras ejecuciones.
- **Detectado en**: cantidad de casos donde apareció (sin nombrarlos).
```

## Ciclo de actualización

1. La skill, al final de cada ejecución, evalúa si surgió un patrón
   generalizable.
2. Antes de agregarlo, valida que no contiene PII ni datos del caso.
3. Si una entrada existente cubre el patrón, **incrementa el contador**
   "detectado en N casos" en lugar de duplicar.
4. Periódicamente (cuando lo solicite el usuario) se condensan entradas
   redundantes.

---

## Errores generales corregidos

> Lista de patrones de error ya identificados y corregidos. Toda skill
> jurídica debe leer esta sección antes de operar.

### [2026-04-25] Inserción de párrafo después de un encabezado de cláusula cae entre el título y su contenido

- **Qué pasaba**: cuando el usuario pide "agrega una cláusula nueva después de la QUINTA", la skill inserta el párrafo justo después del párrafo que contiene "QUINTA. ...", pero ese párrafo suele ser sólo el título; el contenido de la cláusula está en párrafos siguientes. El resultado es una cláusula nueva incrustada entre el título y el contenido de la anterior.
- **Por qué**: la lógica de `insercion_parrafo` toma como ancla el primer párrafo cuyo texto contenga la frase indicada. Para encabezados de cláusula, eso es sólo el título.
- **Cómo evitarlo**: cuando la instrucción del abogado mencione "después de [cláusula]", la skill debe identificar el ÚLTIMO párrafo de la sección referida (siguiente encabezado o fin del documento), no el título. Si Claude no puede determinarlo con seguridad, preguntar al usuario "¿después del título de la cláusula o al final de su contenido?".
- **Detectado en**: 1 caso de prueba (sin nombrar).

### [2026-04-25] Marcadores `{{X}}` reportados como faltas ortográficas

- **Qué pasaba**: el validador ortográfico reportaba `FECHA_INICIO`, `RENTA_MENSUAL`, etc., como errores. Falsos positivos que ahogan la señal real.
- **Por qué**: LanguageTool no reconoce los marcadores tipo `{{X}}` como tokens especiales y los trata como palabras desconocidas.
- **Cómo evitarlo**: filtrar en `validar_ortografia.py` cualquier match cuyo contexto contenga `{{` o `}}`, y cualquier palabra cuyo formato sea `MAYUSCULAS_CON_GUIONES_BAJOS` (patrón típico de marcador). Ya implementado.
- **Detectado en**: todos los casos con plantillas que tienen marcadores.

---

## Sugerencias acumuladas

> Mejoras propuestas (no errores) que el abogado o el sistema han
> identificado durante el uso. Una sugerencia puede promoverse a
> "error corregido" cuando se detecta su omisión causa fricción real.

### [2026-04-25] Validar ambigüedad antes de actuar

Cuando una instrucción contenga verbos vagos como "mejora", "ajusta",
"corrige", "actualiza" sin especificar qué exactamente, la skill debe
preguntar al abogado qué considera por esa acción **antes** de generar
un borrador. Aplicar cambios sobre interpretaciones propias degrada la
confianza del usuario y obliga a re-trabajar.

---

## Convenciones del abogado validadas

> Decisiones estilísticas o de fondo que el abogado ha confirmado en
> varias iteraciones. Estas se aplican por defecto sin volver a preguntar.

*Sin entradas todavía — este documento se poblará con el uso real de las
skills.*

---

## Limitaciones técnicas conocidas

> Restricciones del entorno técnico (OCR, conversión, librerías) y sus
> workarounds. Sirven para que la skill anticipe el problema en lugar
> de fallar y reportar.

### [2026-04-25] Subprocess en Windows con paths que contienen acentos

- **Limitación**: rutas como `C:\Users\herna\Downloads\Jesús\...` rompen `subprocess.run(..., text=True, encoding="utf-8")` cuando el subprocess llamado escribe stderr en cp1252 (encoding por default de la consola Windows).
- **Workaround**: capturar como bytes y decodificar con `errors="replace"`. Aplicado en `cerrar_caso.py` y `test_integrado.py`.

### [2026-04-25] `print()` falla con `UnicodeEncodeError` en consolas cp1252

- **Limitación**: si una cadena contiene `�` (carácter de reemplazo) u otros fuera de cp1252, `print()` falla en Windows.
- **Workaround**: ejecutar Python con `PYTHONIOENCODING=utf-8`, o sanitizar antes de imprimir con `s.encode("ascii", errors="replace").decode("ascii")`.

### [2026-04-25] `language_tool_python` usa snake_case, no camelCase

- **Limitación**: la documentación a veces sugiere camelCase pero la API real es `match.rule_id`, `match.error_length`, `match.offset_in_context`. Usar camelCase produce `AttributeError`.
- **Workaround**: siempre snake_case. Ya documentado en los scripts.

### [2026-04-25] Tesseract requiere los `configs/` además del `traineddata`

- **Limitación**: si se configura `TESSDATA_PREFIX` apuntando a una carpeta custom, los configs de Tesseract (`hocr`, `txt`, `pdf`) no están allí por default — viven en la carpeta original `C:\Program Files\Tesseract-OCR\tessdata\configs\`. Sin ellos, `ocrmypdf` falla silenciosamente.
- **Workaround**: copiar `configs/`, `tessconfigs/` y `script/` desde el tessdata original al custom. Ya documentado en `setup/herramientas-instaladas.md`.

---

## Cómo se garantiza que esté "a la vista"

- Cada `SKILL.md` jurídica declara este archivo como pre-lectura
  obligatoria al inicio de su sección de proceso.
- La memoria persistente `feedback_lecciones_aprendidas.md` apunta a este
  archivo para que Claude lo cargue incluso fuera del contexto de una
  skill formal.
- El comando `/consultar-conocimiento "lecciones"` lo encuentra a través
  de `docs/knowledge/INDEX.md`.
