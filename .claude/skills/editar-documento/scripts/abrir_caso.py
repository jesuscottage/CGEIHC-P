"""abrir_caso.py — Modo A de la skill /editar-documento.

Crea la carpeta de un caso nuevo, asigna ID con correlativo anual,
copia la plantilla seleccionada y genera el caso.yml inicial.

Uso:
    python abrir_caso.py --cliente "Juan García López" \
                         --plantilla arrendamiento-comercial-v2 \
                         [--materia mercantil] \
                         [--descripcion "Demanda por incumplimiento..."]

Devuelve JSON por stdout con id, carpeta y plantilla_copiada.
Sale con código 1 si la plantilla no existe; 2 si el caso ya existía
para ese cliente en el año (avisa pero no sobrescribe).
"""
from __future__ import annotations

import argparse
import json
import shutil
import sys
from datetime import datetime
from pathlib import Path

import yaml

from utils import (
    DESPACHO,
    REPO_ROOT,
    encontrar_plantilla,
    fecha_iso,
    listar_plantillas,
    siguiente_correlativo,
    slugify,
)


def caso_existente_para_cliente(slug_cliente: str, año: int) -> Path | None:
    """Busca si ya existe un caso del año para el mismo slug de cliente."""
    casos_dir = DESPACHO / "casos"
    if not casos_dir.exists():
        return None
    for d in casos_dir.iterdir():
        if d.is_dir() and d.name.startswith(f"{año}-") and d.name.endswith(f"-{slug_cliente}"):
            return d
    return None


def construir_caso_yml(id_caso: str, cliente: str, materia: str | None,
                       descripcion: str | None, plantilla_nombre: str) -> dict:
    return {
        "id": id_caso,
        "cliente": {
            "nombre": cliente,
            "tipo": "persona-fisica",
        },
        "asunto": {
            "materia": materia or "(sin especificar)",
            "descripcion_corta": descripcion or "(pendiente de descripción)",
        },
        "plantilla_origen": {
            "archivo": f"{plantilla_nombre}.docx",
            "fecha_uso": fecha_iso(),
        },
        "fechas": {
            "apertura": fecha_iso(),
            "ultima_modificacion": fecha_iso(),
            "cierre": None,
        },
        "borradores": [],
        "estado": "en-edicion",
        "notas": "",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--cliente", required=True, help="Nombre completo del cliente")
    parser.add_argument("--plantilla", required=True, help="Nombre de la plantilla (sin extensión)")
    parser.add_argument("--materia", default=None, help="Materia jurídica (libre)")
    parser.add_argument("--descripcion", default=None, help="Descripción corta del caso")
    parser.add_argument("--anio", type=int, default=datetime.now().year, help="Año del caso (default: año actual)")
    parser.add_argument("--forzar", action="store_true", help="Crea caso aunque ya exista uno para el mismo cliente en el año")
    args = parser.parse_args()

    plantilla_path = encontrar_plantilla(args.plantilla)
    if plantilla_path is None:
        disponibles = listar_plantillas()
        sys.stderr.write(
            f"ERROR: plantilla '{args.plantilla}' no encontrada en despacho/plantillas/.\n"
        )
        if disponibles:
            sys.stderr.write("Plantillas disponibles:\n")
            for p in disponibles:
                sys.stderr.write(f"  - {p['nombre']}\n")
        else:
            sys.stderr.write("No hay plantillas registradas todavía.\n")
        return 1

    slug = slugify(args.cliente)
    casos_dir = DESPACHO / "casos"
    casos_dir.mkdir(parents=True, exist_ok=True)

    existente = caso_existente_para_cliente(slug, args.anio)
    if existente and not args.forzar:
        sys.stderr.write(
            f"AVISO: ya existe un caso para '{args.cliente}' en {args.anio}: {existente.name}\n"
            "Usa --forzar para crear uno nuevo de todos modos.\n"
        )
        return 2

    correlativo = siguiente_correlativo(args.anio, casos_dir)
    id_caso = f"{args.anio}-{correlativo:03d}-{slug}"
    caso_dir = casos_dir / id_caso

    (caso_dir / "00-input" / "adjuntos").mkdir(parents=True, exist_ok=True)
    (caso_dir / "01-plantilla").mkdir(parents=True, exist_ok=True)
    (caso_dir / "02-borradores").mkdir(parents=True, exist_ok=True)
    (caso_dir / "04-final").mkdir(parents=True, exist_ok=True)

    destino_plantilla = caso_dir / "01-plantilla" / "plantilla-origen.docx"
    shutil.copy2(plantilla_path, destino_plantilla)

    datos = construir_caso_yml(id_caso, args.cliente, args.materia, args.descripcion, args.plantilla)
    with (caso_dir / "caso.yml").open("w", encoding="utf-8") as f:
        yaml.safe_dump(datos, f, allow_unicode=True, sort_keys=False, indent=2, default_flow_style=False)

    resultado = {
        "id": id_caso,
        "carpeta": str(caso_dir.relative_to(REPO_ROOT)).replace("\\", "/"),
        "plantilla_origen": str(plantilla_path.relative_to(REPO_ROOT)).replace("\\", "/"),
        "plantilla_copiada": str(destino_plantilla.relative_to(REPO_ROOT)).replace("\\", "/"),
    }
    print(json.dumps(resultado, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
