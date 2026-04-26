"""validar_ortografia.py — Valida ortografía y gramática del DOCX en español.

Usa language_tool_python (LanguageTool 6.x sobre Java). Es informativo:
nunca bloquea el flujo; sólo devuelve la lista de observaciones para
incluirlas en el .diff.md.

Uso:
    python validar_ortografia.py --docx archivo.docx [--max-observaciones 100]

Salida JSON:
    {
      "total_observaciones": 12,
      "observaciones": [
        {
          "texto": "redacto",
          "sugerencias": ["redactó"],
          "regla": "MORFOLOGIK_RULE_ES",
          "contexto": "...el abogado redacto el documento..."
        },
        ...
      ]
    }

Filtra reglas conocidas como falsos positivos en jerga jurídica
(p. ej. "C. JUEZ" como saludo formal).
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from docx import Document

# Reglas a ignorar — son falsos positivos comunes en documentos jurídicos
REGLAS_IGNORAR = {
    "WHITESPACE_RULE",      # falsos positivos por espacios en encabezados procesales
    "UPPERCASE_SENTENCE_START",  # encabezados intencionalmente en mayúsculas
    "ES_QUESTION_MARK",     # puntuación abreviada típica de citas
}

# Patrones de palabras que la skill no debe marcar (siglas jurídicas comunes)
PALABRAS_IGNORAR = {
    "C.",       # forma de tratamiento ("C. JUEZ...")
    "Lic.",     # licenciado
    "L.",       # licenciado abreviado
    "art.",     # artículo
    "frac.",    # fracción
    "núm.",     # número
}


def _extraer_texto_para_validacion(docx_path: Path) -> str:
    """Extrae el texto del cuerpo del documento (sin tablas ni headers/footers).

    Las tablas suelen tener fragmentos sueltos que confunden al validador
    gramatical. Headers/footers también.
    """
    doc = Document(str(docx_path))
    parrafos = [p.text for p in doc.paragraphs if p.text.strip()]
    return "\n".join(parrafos)


def _filtrar_observacion(match, texto_completo: str) -> bool:
    """True si la observación es válida y debe reportarse."""
    if match.rule_id in REGLAS_IGNORAR:
        return False
    inicio = match.offset
    fin = inicio + match.error_length
    fragmento = texto_completo[inicio:fin]
    if fragmento.strip() in PALABRAS_IGNORAR:
        return False
    # Ignorar marcadores de plantilla {{X}} y palabras dentro de {{...}}
    contexto_amplio = texto_completo[max(0, inicio - 2):min(len(texto_completo), fin + 2)]
    if "{{" in contexto_amplio or "}}" in contexto_amplio:
        return False
    # Ignorar palabras que parecen identificadores de plantilla (MAYUSCULAS_CON_GUIONES)
    import re as _re
    if _re.fullmatch(r"[A-Z][A-Z_]+[A-Z]", fragmento.strip()):
        return False
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--docx", required=True, help="Ruta al DOCX a validar")
    parser.add_argument("--max-observaciones", type=int, default=100,
                        help="Máximo de observaciones a reportar (default: 100)")
    args = parser.parse_args()

    docx = Path(args.docx).resolve()
    if not docx.exists():
        sys.stderr.write(f"ERROR: no existe {docx}\n")
        return 1

    texto = _extraer_texto_para_validacion(docx)
    if not texto.strip():
        print(json.dumps({"total_observaciones": 0, "observaciones": []}, ensure_ascii=False, indent=2))
        return 0

    try:
        import language_tool_python
    except ImportError:
        sys.stderr.write("ERROR: language_tool_python no instalado\n")
        return 1

    try:
        tool = language_tool_python.LanguageTool("es")
    except Exception as e:
        sys.stderr.write(f"ERROR: no se pudo iniciar LanguageTool: {e}\n")
        sys.stderr.write("Verifica que Java esté en el PATH (source setup/env-tools.sh)\n")
        return 1

    try:
        matches = tool.check(texto)
    finally:
        tool.close()

    observaciones = []
    for m in matches:
        if not _filtrar_observacion(m, texto):
            continue
        contexto_inicio = max(0, m.offset_in_context - 30)
        contexto_fin = min(len(m.context), m.offset_in_context + m.error_length + 30)
        observaciones.append({
            "texto": texto[m.offset:m.offset + m.error_length],
            "sugerencias": m.replacements[:3],
            "regla": m.rule_id,
            "mensaje": m.message,
            "contexto": m.context[contexto_inicio:contexto_fin].strip(),
        })
        if len(observaciones) >= args.max_observaciones:
            break

    print(json.dumps({
        "total_observaciones": len(observaciones),
        "observaciones": observaciones,
    }, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
