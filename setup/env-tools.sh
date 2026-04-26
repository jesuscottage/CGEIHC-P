#!/usr/bin/env bash
# env-tools.sh — Activa el PATH y variables de entorno para las herramientas
# del proyecto. Uso: `source setup/env-tools.sh`
#
# Esto NO es necesario en sesiones nuevas de bash/PowerShell/Terminal abiertas
# después de la instalación inicial — el PATH y JAVA_HOME ya están persistidos
# a nivel usuario. Sirve sólo cuando se opera desde un proceso heredado que
# no recargó el entorno.

# Pandoc (instalado per-user)
export PATH="/c/Users/herna/AppData/Local/pandoc/pandoc-3.9.0.2:$PATH"

# Tesseract OCR
export PATH="/c/Program Files/Tesseract-OCR:$PATH"
export TESSDATA_PREFIX='C:\Users\herna\AppData\Local\tessdata'

# qpdf
export PATH="/c/Program Files/qpdf 12.3.2/bin:$PATH"

# ImageMagick
export PATH="/c/Program Files/ImageMagick-7.1.2-Q16-HDRI:$PATH"

# Java (necesario para language_tool_python)
export PATH="/c/Program Files/Eclipse Adoptium/jre-21.0.10.7-hotspot/bin:$PATH"
export JAVA_HOME='C:\Program Files\Eclipse Adoptium\jre-21.0.10.7-hotspot'

# Ghostscript (necesario para ocrmypdf)
export PATH="/c/Program Files/gs/gs10.07.0/bin:$PATH"

# LibreOffice (fallback DOCX→PDF headless sin bloquear Word)
export PATH="/c/Program Files/LibreOffice/program:$PATH"

echo "Entorno de herramientas activado:"
echo "  pandoc       $(command -v pandoc 2>/dev/null || echo NO)"
echo "  tesseract    $(command -v tesseract 2>/dev/null || echo NO)"
echo "  java         $(command -v java 2>/dev/null || echo NO)"
echo "  qpdf         $(command -v qpdf 2>/dev/null || echo NO)"
echo "  magick       $(command -v magick 2>/dev/null || echo NO)"
echo "  gswin64c     $(command -v gswin64c 2>/dev/null || echo NO)"
echo "  soffice      $(command -v soffice 2>/dev/null || echo NO)"
echo "  TESSDATA_PREFIX = $TESSDATA_PREFIX"

# Nota: el TESSDATA_PREFIX de usuario contiene spa/eng/osd + configs
# (hocr, txt, pdf, etc.) copiados desde C:\Program Files\Tesseract-OCR\tessdata.
# Si Tesseract se actualiza, re-copiar los configs.
