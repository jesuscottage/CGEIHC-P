"""cerrar_caso.py — Modo C de la skill /editar-documento.

Cierra un caso: copia el último borrador a 04-final/, regenera PDF,
publica copia en despacho/entregables/{AAAA}/{MM}/, actualiza INDICE
y marca el caso como cerrado en caso.yml.

Uso:
    python cerrar_caso.py --caso 2026-001-... [--motor word|libreoffice]

Sale con código 1 si el caso no existe o ya está cerrado.
Sale con código 2 si no hay borradores que cerrar.
"""
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

from utils import (
    DESPACHO,
    REPO_ROOT,
    encontrar_caso,
    escribir_caso_yml,
    fecha_iso,
    leer_caso_yml,
    ruta_ultimo_borrador,
)


def _convertir_a_pdf(docx: Path, pdf: Path, motor: str) -> str:
    """Llama a convertir_pdf.py y devuelve el motor usado."""
    script = Path(__file__).parent / "convertir_pdf.py"
    resultado = subprocess.run(
        [sys.executable, str(script), "--docx", str(docx), "--salida", str(pdf), "--motor", motor],
        capture_output=True,
    )
    stdout = resultado.stdout.decode("utf-8", errors="replace") if resultado.stdout else ""
    stderr = resultado.stderr.decode("utf-8", errors="replace") if resultado.stderr else ""
    if resultado.returncode != 0:
        raise RuntimeError(f"Conversión PDF falló: {stderr}")
    if not stdout.strip():
        raise RuntimeError(f"Conversión PDF no produjo salida JSON. stderr: {stderr[:300]}")
    info = json.loads(stdout)
    return info["motor_usado"]


def _agregar_a_indice_entregables(caso_id: str, cliente: str, materia: str,
                                    fecha_cierre: str, pdf_publicado: Path) -> None:
    """Inserta una fila en despacho/entregables/INDICE.md."""
    indice = DESPACHO / "entregables" / "INDICE.md"
    if not indice.exists():
        return
    contenido = indice.read_text(encoding="utf-8")

    fila_nueva = (
        f"| {caso_id} | {cliente} | {materia} | {fecha_cierre} | "
        f"[{pdf_publicado.name}]({pdf_publicado.relative_to(DESPACHO / 'entregables').as_posix()}) |"
    )

    placeholder = "| — | — | (sin entregables todavía) | — | — |"
    if placeholder in contenido:
        nuevo = contenido.replace(placeholder, fila_nueva)
    else:
        # Insertar antes del primer "---" después de la tabla
        lineas = contenido.split("\n")
        idx_tabla = None
        for i, l in enumerate(lineas):
            if l.startswith("| Caso |"):
                idx_tabla = i
                break
        if idx_tabla is None:
            return
        # Buscar última fila de la tabla
        i = idx_tabla + 2  # saltar header y separador
        while i < len(lineas) and lineas[i].startswith("|"):
            i += 1
        lineas.insert(i, fila_nueva)
        nuevo = "\n".join(lineas)

    # Actualizar fecha de "Última actualización"
    nuevo = _actualizar_fecha_indice(nuevo)

    indice.write_text(nuevo, encoding="utf-8")


def _actualizar_fecha_indice(contenido: str) -> str:
    import re
    return re.sub(
        r"^> Última actualización: .*$",
        f"> Última actualización: {fecha_iso()}",
        contenido,
        count=1,
        flags=re.MULTILINE,
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--caso", required=True, help="ID del caso")
    parser.add_argument("--motor", choices=["auto", "word", "libreoffice"], default="auto",
                        help="Motor para regenerar PDF final (default: auto)")
    args = parser.parse_args()

    caso_dir = encontrar_caso(args.caso)
    if caso_dir is None:
        sys.stderr.write(f"ERROR: caso '{args.caso}' no encontrado\n")
        return 1

    datos = leer_caso_yml(caso_dir)
    if datos.get("estado") == "cerrado":
        sys.stderr.write(f"ERROR: el caso '{args.caso}' ya está cerrado.\n")
        sys.stderr.write("Si necesitas reabrirlo, edita manualmente caso.yml (estado: en-edicion).\n")
        return 1

    ultimo = ruta_ultimo_borrador(caso_dir)
    if ultimo is None:
        sys.stderr.write(f"ERROR: el caso '{args.caso}' no tiene borradores. Nada que cerrar.\n")
        return 2

    final_dir = caso_dir / "04-final"
    final_dir.mkdir(exist_ok=True)
    final_docx = final_dir / "final.docx"
    final_pdf = final_dir / "final.pdf"

    shutil.copy2(ultimo, final_docx)

    try:
        motor_usado = _convertir_a_pdf(final_docx, final_pdf, args.motor)
    except Exception as e:
        sys.stderr.write(f"ERROR al generar PDF final: {e}\n")
        return 1

    # Publicar en entregables/{AAAA}/{MM}/
    ahora = datetime.now()
    publicacion_dir = DESPACHO / "entregables" / f"{ahora.year}" / f"{ahora.month:02d}"
    publicacion_dir.mkdir(parents=True, exist_ok=True)
    nombre_pub_docx = f"{datos['id']}-final.docx"
    nombre_pub_pdf = f"{datos['id']}-final.pdf"
    pub_docx = publicacion_dir / nombre_pub_docx
    pub_pdf = publicacion_dir / nombre_pub_pdf
    shutil.copy2(final_docx, pub_docx)
    shutil.copy2(final_pdf, pub_pdf)

    # Actualizar caso.yml
    datos["estado"] = "cerrado"
    datos.setdefault("fechas", {})["cierre"] = fecha_iso()
    datos["fechas"]["ultima_modificacion"] = fecha_iso()
    escribir_caso_yml(caso_dir, datos)

    # Actualizar INDICE de entregables
    _agregar_a_indice_entregables(
        caso_id=datos["id"],
        cliente=datos.get("cliente", {}).get("nombre", "(sin nombre)"),
        materia=datos.get("asunto", {}).get("materia", "(sin materia)"),
        fecha_cierre=fecha_iso(),
        pdf_publicado=pub_pdf,
    )

    resultado = {
        "id": datos["id"],
        "borrador_origen": str(ultimo.relative_to(REPO_ROOT)).replace("\\", "/"),
        "final_docx": str(final_docx.relative_to(REPO_ROOT)).replace("\\", "/"),
        "final_pdf": str(final_pdf.relative_to(REPO_ROOT)).replace("\\", "/"),
        "publicacion_docx": str(pub_docx.relative_to(REPO_ROOT)).replace("\\", "/"),
        "publicacion_pdf": str(pub_pdf.relative_to(REPO_ROOT)).replace("\\", "/"),
        "motor_pdf": motor_usado,
        "fecha_cierre": fecha_iso(),
    }
    print(json.dumps(resultado, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
