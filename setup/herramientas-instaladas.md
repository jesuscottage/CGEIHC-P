---
title: Herramientas instaladas — inventario y verificación
date: 2026-04-25
estado: completo — todas las herramientas instaladas y verificadas
---

# Herramientas instaladas

> Inventario del entorno tras la instalación inicial.
> Generado el 2026-04-25 sobre Windows 11 Home Single Language.

## Resumen ejecutivo

| Estado | Conteo |
|---|---|
| ✅ Instalado y verificado | 15 |

El flujo principal de la skill `/editar-documento` está **100% operativo**:
- **Word como motor PDF principal** (preferencia del usuario, vía COM).
- **LibreOffice headless** disponible como fallback que no bloquea Word.
- **Tesseract + Ghostscript + ocrmypdf** completos para OCR de plantillas escaneadas.
- **language_tool_python** verificando ortografía y gramática españolas.

---

## Herramientas instaladas y verificadas

| Herramienta | Versión | Ubicación | Verificación |
|---|---|---|---|
| Python | 3.12.3 | `C:\Users\herna\AppData\Local\Programs\Python\Python312\` | ✅ |
| Pandoc | 3.9.0.2 | `C:\Users\herna\AppData\Local\pandoc\pandoc-3.9.0.2\` | ✅ MD→DOCX OK |
| Tesseract OCR | 5.4.0 | `C:\Program Files\Tesseract-OCR\` | ✅ OCR español OK |
| Tessdata `spa`, `eng`, `osd` | best | `C:\Users\herna\AppData\Local\tessdata\` | ✅ |
| Microsoft Word | 16.0 (Office16) | `C:\Program Files\Microsoft Office\root\Office16\` | ✅ via COM |
| Eclipse Temurin JRE | 21.0.10 | `C:\Program Files\Eclipse Adoptium\jre-21.0.10.7-hotspot\` | ✅ |
| qpdf | 12.3.2 | `C:\Program Files\qpdf 12.3.2\bin\` | ✅ |
| ImageMagick | 7.1.2-21 Q16-HDRI | `C:\Program Files\ImageMagick-7.1.2-Q16-HDRI\` | ✅ |
| python-docx | 1.2.0 | pip | ✅ DOCX read/write |
| docx2pdf + pywin32 | 0.1.8 / 311 | pip | ✅ DOCX→PDF via Word |
| pdfplumber | 0.11.9 | pip | ✅ |
| pypdf | 6.10.2 | pip | ✅ |
| mammoth | 1.12.0 | pip | ✅ DOCX→HTML/MD |
| language_tool_python | 3.3.0 + LT 6.8 | pip | ✅ Detecta tildes faltantes |
| pyspellchecker | 0.9.0 | pip | ✅ |
| weasyprint | 12.2 | pip (preexistente) | ✅ HTML→PDF |
| Pillow | 12.2.0 | pip | ✅ |
| reportlab | 4.4.10 | pip | ✅ |
| ocrmypdf | 17.4.2 | pip | ✅ End-to-end PDF→OCR-PDF OK |
| Ghostscript | 10.07.0 | `C:\Program Files\gs\gs10.07.0\bin\` | ✅ |
| LibreOffice | 26.2.2 | `C:\Program Files\LibreOffice\program\` | ✅ Headless DOCX→PDF OK |
| fix-spanish-ortho.sh | — | `scripts/` (preexistente) | ✅ Sólo `.md`, sólo tildes |

---

## Variables de entorno persistidas (nivel usuario)

```
PATH += C:\Users\herna\AppData\Local\pandoc\pandoc-3.9.0.2
PATH += C:\Program Files\Tesseract-OCR
PATH += C:\Program Files\qpdf 12.3.2\bin
PATH += C:\Program Files\ImageMagick-7.1.2-Q16-HDRI
PATH += C:\Program Files\Eclipse Adoptium\jre-21.0.10.7-hotspot\bin
PATH += C:\Program Files\gs\gs10.07.0\bin
PATH += C:\Program Files\LibreOffice\program
JAVA_HOME = C:\Program Files\Eclipse Adoptium\jre-21.0.10.7-hotspot
TESSDATA_PREFIX = C:\Users\herna\AppData\Local\tessdata
```

> Nota sobre `TESSDATA_PREFIX`: contiene `spa.traineddata`, `eng.traineddata`,
> `osd.traineddata` y los **configs** (`hocr`, `txt`, `pdf`, etc.) copiados
> desde `C:\Program Files\Tesseract-OCR\tessdata\configs\`. Sin esos configs,
> `ocrmypdf` falla. Si Tesseract se actualiza, re-copiar los configs.

> **Importante**: estas variables sólo aplican a procesos lanzados *después*
> de configurarlas. Para esta sesión actual de Claude Code (que ya estaba
> corriendo cuando se persistieron), usar `source setup/env-tools.sh` antes
> de cualquier comando que las requiera, o reiniciar la terminal.

---

## Verificaciones funcionales realizadas

| # | Test | Resultado |
|---|------|-----------|
| 1 | `pandoc test.md -o test.docx` | ✅ DOCX 10 KB válido |
| 2 | `tesseract test.png -l spa` sobre imagen "México otorgó la jurisdicción" | ✅ Texto extraído íntegro con tildes |
| 3 | `python-docx` escribe → `docx2pdf` convierte vía Word COM | ✅ PDF 118 KB generado |
| 4 | `language_tool_python` sobre frase con 3 tildes faltantes | ✅ Detectó "redacto"→"redactó", "Tambien"→"También", "parecio"→"pareció" |
| 5 | `ocrmypdf -l spa --force-ocr` end-to-end PDF→PDF (con Ghostscript) | ✅ Texto recuperable con `pdftotext` |
| 6 | `soffice --headless --convert-to pdf` sobre DOCX | ✅ PDF 37 KB generado sin abrir UI |

---

## Notas de operación

- **Conectividad**: el CDN principal de LibreOffice
  (`download.documentfoundation.org`) está bloqueado a nivel de red TCP/443
  desde esta máquina. Por eso el `winget install` falló dos veces. Se usó
  el mirror `ftp.osuosl.org` para la descarga, y el `.msi` quedó archivado
  en `setup/instaladores/` por si hay que reinstalar sin red.
- **LibreOffice MSI** retorna exit code 3010 (instalación correcta + reinicio
  recomendado para liberar archivos de sistema). El reinicio es opcional —
  `soffice` ya funciona en headless sin reiniciar.

## Conflictos menores conocidos

Tras instalar `ocrmypdf` se actualizó `huggingface-hub` a 1.2.3, lo cual genera
warnings en `tokenizers` y `transformers` (instalados en el entorno por otros
proyectos del usuario). **No afecta a este proyecto**: ningún workflow jurídico
usa esas librerías. Si el usuario detecta problemas en otros proyectos:
`pip install huggingface-hub<1.0`.

---

## Cobertura por fase del flujo `/editar-documento`

| Fase | Herramienta principal | Estado |
|---|---|---|
| Lectura DOCX | python-docx | ✅ |
| Lectura PDF nativo | pdfplumber, pdftotext | ✅ |
| Lectura PDF escaneado | tesseract (`-l spa`) | ✅ |
| Manipulación interna | python-docx (default) / pandoc para casos prosa | ✅ |
| Salida DOCX | python-docx | ✅ |
| Salida PDF (alta fidelidad) | docx2pdf via Word COM | ✅ |
| Salida PDF (fallback headless) | LibreOffice | 🔴 pendiente |
| Verificación ortográfica | language_tool_python + fix-spanish-ortho.sh | ✅ |
| Diff entre borradores | pandoc `--track-changes` / diff plano | ✅ |
| Auxiliares PDF (split/merge/repair) | qpdf | ✅ |
| Reconstrucción OCR de PDF | ocrmypdf | 🟡 requiere GS |
| Procesado de imágenes para OCR | ImageMagick + Pillow | ✅ |
