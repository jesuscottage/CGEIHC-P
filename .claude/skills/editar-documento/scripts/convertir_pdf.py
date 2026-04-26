"""convertir_pdf.py — Conversión DOCX → PDF.

Motor preferido: Microsoft Word vía docx2pdf (COM). Si Word está ocupado,
no responde, o falla por cualquier razón, hace fallback a LibreOffice
headless (`soffice --headless --convert-to pdf`).

Uso:
    python convertir_pdf.py --docx archivo.docx [--salida archivo.pdf]

Reporta JSON por stdout:
    {"motor_usado": "word" | "libreoffice", "pdf": "ruta", "tiempo_s": 4.1}

Sale con código 1 si ambos motores fallan.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path


def _convertir_con_word(docx: Path, pdf: Path) -> bool:
    """Intenta conversión con docx2pdf (Word COM). True si tuvo éxito."""
    try:
        from docx2pdf import convert
        convert(str(docx), str(pdf))
        return pdf.exists() and pdf.stat().st_size > 0
    except Exception:
        return False


def _convertir_con_libreoffice(docx: Path, pdf: Path) -> bool:
    """Intenta conversión con LibreOffice headless. True si tuvo éxito."""
    posibles = [
        "soffice",
        r"C:\Program Files\LibreOffice\program\soffice.exe",
        r"C:\Program Files (x86)\LibreOffice\program\soffice.exe",
    ]
    soffice = None
    for cmd in posibles:
        if cmd == "soffice":
            from shutil import which
            ruta = which(cmd)
            if ruta:
                soffice = ruta
                break
        else:
            if Path(cmd).exists():
                soffice = cmd
                break
    if soffice is None:
        return False

    salida_dir = pdf.parent
    salida_dir.mkdir(parents=True, exist_ok=True)

    cmd = [soffice, "--headless", "--convert-to", "pdf", "--outdir", str(salida_dir), str(docx)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
    except subprocess.TimeoutExpired:
        return False
    if result.returncode != 0:
        return False

    # LibreOffice nombra el PDF como {basename}.pdf en outdir
    generado = salida_dir / (docx.stem + ".pdf")
    if not generado.exists():
        return False
    if generado != pdf:
        if pdf.exists():
            pdf.unlink()
        generado.rename(pdf)
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--docx", required=True, help="Ruta al DOCX a convertir")
    parser.add_argument("--salida", default=None, help="Ruta del PDF de salida (default: mismo nombre, .pdf)")
    parser.add_argument("--motor", choices=["auto", "word", "libreoffice"], default="auto",
                        help="Motor a usar (default: auto = word con fallback)")
    parser.add_argument("--reintentos-word", type=int, default=2,
                        help="Reintentos con Word antes del fallback (default: 2)")
    args = parser.parse_args()

    docx = Path(args.docx).resolve()
    if not docx.exists():
        sys.stderr.write(f"ERROR: no existe {docx}\n")
        return 1

    pdf = Path(args.salida).resolve() if args.salida else docx.with_suffix(".pdf")
    pdf.parent.mkdir(parents=True, exist_ok=True)

    inicio = time.time()
    motor_usado = None

    if args.motor in ("auto", "word"):
        for intento in range(1, args.reintentos_word + 1):
            if _convertir_con_word(docx, pdf):
                motor_usado = "word"
                break
            if intento < args.reintentos_word:
                time.sleep(2)

    if motor_usado is None and args.motor in ("auto", "libreoffice"):
        if _convertir_con_libreoffice(docx, pdf):
            motor_usado = "libreoffice"

    if motor_usado is None:
        sys.stderr.write("ERROR: ambos motores fallaron (Word y LibreOffice)\n")
        return 1

    transcurrido = round(time.time() - inicio, 2)
    print(json.dumps({
        "motor_usado": motor_usado,
        "pdf": str(pdf).replace(os.sep, "/"),
        "tiempo_s": transcurrido,
        "tamano_bytes": pdf.stat().st_size,
    }, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
