"""Genera los archivos MP3 de narración para cada módulo del museo."""
import asyncio
import os
import edge_tts

AUDIO_DIR = os.path.join(os.path.dirname(__file__), "..", "app", "assets", "audio")
os.makedirs(AUDIO_DIR, exist_ok=True)

# Voz masculina profunda en español (es-ES-AlvaroNeural es la más grave disponible)
VOZ   = "es-ES-AlvaroNeural"
RITMO = "-15%"   # más lento = más peso narrativo
TONO  = "-8Hz"   # más grave, parecido a Morgan Freeman

NARRACIONES = {
    "narr_M1_IZQ": (
        "Deshielo del Ártico. "
        "El Ártico está perdiendo más del trece por ciento de su superficie de hielo marino "
        "cada década... un ritmo sin precedentes en miles de años. "
        "Las temperaturas en esta región aumentan hasta cuatro veces más rápido que en el "
        "resto del planeta, derritiendo glaciares milenarios que tardarían siglos en formarse de nuevo. "
        "Este deshielo eleva el nivel global del mar, amenazando comunidades costeras en todos los continentes. "
        "Además, altera las corrientes oceánicas que regulan el clima de Europa y América, "
        "provocando fenómenos meteorológicos extremos cada vez más frecuentes e intensos."
    ),
    "narr_M2_IZQ": (
        "Fauna en Peligro. "
        "El oso polar, símbolo del Ártico, depende del hielo marino para cazar focas y reproducirse. "
        "Con menos hielo cada año, estos animales deben nadar distancias cada vez mayores, "
        "agotando sus reservas energéticas. "
        "Se estima que dos tercios de la población mundial de osos polares podría desaparecer "
        "antes del año dos mil cincuenta si el calentamiento continúa al ritmo actual. "
        "Morsas, ballenas, zorros árticos y cientos de especies migratorias también se ven afectadas. "
        "La pérdida de un eslabón en esta cadena alimentaria tiene consecuencias en cascada "
        "para todo el ecosistema ártico."
    ),
    "narr_M3_IZQ": (
        "Ciudades Inundadas. "
        "Si las emisiones globales no se reducen drásticamente, el nivel del mar podría subir "
        "entre medio metro y un metro treinta centímetros para el año dos mil cien. "
        "Ciudades como Miami, Nueva Orleans, Tokio, Ámsterdam, Bombay y Shanghái enfrentarían "
        "inundaciones permanentes que harían inviable la vida de millones de personas. "
        "Se calcula que más de seiscientos ochenta millones de personas viven en zonas costeras "
        "de baja elevación. "
        "El desplazamiento masivo generaría conflictos por recursos, colapso de infraestructuras "
        "y pérdidas económicas que superarían los catorce billones de dólares anuales."
    ),
    "narr_M1_DER": (
        "Energía Eólica. "
        "Los aerogeneradores modernos pueden abastecer de energía completamente limpia a miles de hogares "
        "sin emitir un solo gramo de dióxido de carbono durante su operación. "
        "La energía eólica es hoy una de las fuentes de electricidad más baratas del mundo, "
        "con costos que han caído más del setenta por ciento en la última década. "
        "En dos mil veintitrés, la capacidad eólica global superó los mil gigavatios, "
        "suficiente para abastecer a más de setecientos millones de hogares. "
        "Países como Dinamarca ya generan más del cincuenta por ciento de su electricidad con el viento, "
        "demostrando que una transición energética completa es técnica y económicamente posible."
    ),
    "narr_M2_DER": (
        "Auto Eléctrico. "
        "El transporte representa casi el veinticinco por ciento de las emisiones globales de dióxido de carbono. "
        "Sustituir los vehículos de gasolina por eléctricos alimentados con energía renovable "
        "podría eliminar hasta el ochenta por ciento de esas emisiones, "
        "transformando radicalmente la huella de carbono del sector. "
        "Las ventas de autos eléctricos crecieron un treinta y cinco por ciento en dos mil veintitrés "
        "y se espera que superen a los de combustión interna antes de dos mil treinta y cinco. "
        "Además de reducir emisiones, los vehículos eléctricos disminuyen la contaminación del aire "
        "en ciudades, salvando cientos de miles de vidas cada año."
    ),
    "narr_M3_DER": (
        "Captura de Carbono. "
        "Las tecnologías de captura directa de aire extraen dióxido de carbono directamente de la atmósfera "
        "y lo comprimen para almacenarlo de forma permanente en formaciones geológicas profundas. "
        "Aunque todavía en etapa de escalamiento, estas plantas representan una herramienta crítica "
        "para revertir el daño ya causado al clima. "
        "Combinadas con la reforestación masiva y la reducción de emisiones, "
        "la captura de carbono podría ayudar a alcanzar la neutralidad de carbono antes de dos mil cincuenta. "
        "Islandia ya opera la planta más grande del mundo, capturando el equivalente "
        "a lo que emiten miles de automóviles cada año."
    ),
    "narr_M5": (
        "Acuerdos por el Clima. "
        "El Acuerdo de París, firmado en dos mil quince, une a ciento noventa y seis naciones "
        "en un compromiso histórico: limitar el calentamiento global a un grado y medio "
        "por encima de los niveles preindustriales. "
        "Este umbral es crítico... superarlo significaría consecuencias irreversibles "
        "para ecosistemas, comunidades y economías de todo el planeta. "
        "Cada nación presenta sus propios planes de reducción de emisiones, "
        "y cada cinco años se revisan y fortalecen esos compromisos. "
        "La ciencia, la tecnología y la voluntad política colectiva son los tres pilares "
        "sobre los que descansa la única salida posible: "
        "un futuro donde el Ártico vuelva a congelarse, las ciudades permanezcan en pie, "
        "y todas las especies encuentren un hogar donde vivir."
    ),
}

async def generar(nombre, texto):
    ruta = os.path.join(AUDIO_DIR, nombre + ".mp3")
    comunicador = edge_tts.Communicate(texto, VOZ, rate=RITMO, pitch=TONO)
    await comunicador.save(ruta)
    print(f"  OK  {nombre}.mp3")

async def main():
    print(f"Generando {len(NARRACIONES)} archivos en:\n  {AUDIO_DIR}\n")
    for nombre, texto in NARRACIONES.items():
        await generar(nombre, texto)
    print("\nListo.")

if __name__ == "__main__":
    asyncio.run(main())
