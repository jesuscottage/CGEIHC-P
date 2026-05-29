"""Genera el archivo de proyecto InstallForge (.ifp) basado en el template real."""
import os
import glob

DIST_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'dist')
DIST_DIR = os.path.abspath(DIST_DIR)
OUT_IFP = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'entregables', 'NuestroMundo.ifp')
OUT_IFP = os.path.abspath(OUT_IFP)
SETUP_EXE = r'C:\Users\herna\Downloads\NuestroMundo_Setup.exe'

# Recoger archivos individuales (no carpetas) de dist/
files_xml = []
folders_in_dist = set()
for dirpath, dirnames, filenames in os.walk(DIST_DIR):
    dirnames.sort()
    rel_dir = os.path.relpath(dirpath, DIST_DIR)
    if rel_dir != '.':
        # Es una subcarpeta — agregar como folder
        folders_in_dist.add(rel_dir)

# Archivos sueltos en la raiz de dist/
root_files = []
for f in sorted(os.listdir(DIST_DIR)):
    full = os.path.join(DIST_DIR, f)
    if os.path.isfile(full):
        size = os.path.getsize(full)
        if size > 1024*1024:
            size_str = f"{size/1024/1024:.1f} MB"
        else:
            size_str = f"{size/1024:.1f} KB"
        ext = os.path.splitext(f)[1].lstrip('.')
        root_files.append(f'      <InstallationFile size="{size_str}" type="{ext}">{full}</InstallationFile>')

# Carpetas de primer nivel en dist/
top_folders = []
for f in sorted(os.listdir(DIST_DIR)):
    full = os.path.join(DIST_DIR, f)
    if os.path.isdir(full):
        top_folders.append(f'      <InstallationFile size="N/A" type="[Folder]">{full}\\</InstallationFile>')

installation_files = '\n'.join(root_files + top_folders)

ifp_content = f'''<?xml version="1.0" encoding="UTF-8"?>

<InstallForgeProject>
  <Metadata productVersion="1.6.1" projectFileVersion="2.0"/>
  <ProductInformation>
    <Name>Nuestro Mundo - Polo Norte</Name>
    <Version>1.0.0</Version>
    <CompanyName>UNAM Facultad de Ingenieria - CGEIHC 2026-2</CompanyName>
    <WebURI>https://</WebURI>
  </ProductInformation>
  <InstallerConfiguration>
    <Prerequisites>
      <OperatingSystem check="true">
        <WindowsXP>false</WindowsXP>
        <WindowsServer2003>false</WindowsServer2003>
        <WindowsVista>false</WindowsVista>
        <WindowsServer2008>false</WindowsServer2008>
        <Windows7>false</Windows7>
        <Windows8>false</Windows8>
        <Windows10>true</Windows10>
        <WindowsServer2016>false</WindowsServer2016>
        <WindowsServer2019>false</WindowsServer2019>
        <WindowsServer2022>false</WindowsServer2022>
        <Windows11>true</Windows11>
        <WindowsServer2025>false</WindowsServer2025>
      </OperatingSystem>
    </Prerequisites>
    <UserInterface>
      <WizardImage filePath="&lt;main&gt;"/>
      <HeaderImage filePath="&lt;main&gt;"/>
      <Label isEnabled="true"/>
      <VisualStyle isEnabled="true"/>
    </UserInterface>
    <InstallationFiles defaultInstallationPath="&lt;ProgramFiles&gt;\\NuestroMundo" isDefaultInstallationPathAlterable ="true">
{installation_files}
    </InstallationFiles>
    <Languages/>
    <Variables/>
    <Commands/>
    <Registry/>
    <Shortcuts defaultPath="&lt;Company&gt;\\&lt;AppName&gt;\\" isDefaultPathAlterable="false" isCreateStartmenuShortcutsForAllUsers="false" isCreateDesktopShortcutsForAllUsers="false">
      <Shortcut destination="Desktop" name="Nuestro Mundo - Polo Norte" targetFile="&lt;InstallPath&gt;\\CGEIHC.exe" commandLineArguments="" iconFilePath="" iconIndex="0"/>
    </Shortcuts>
    <Serials isDialogEnabled="false" count="1000" mask ="#####-#####-#####-#####"/>
    <SplashScreen isDialogEnabled="false" delayTime="2">
      <Image filePath=""/>
      <Sound isEnabled="false" filePath=""/>
    </SplashScreen>
    <License isDialogEnabled="false">
      <Data>e1xydGYxXGFuc2lcZGVmZjB7XGZvbnR0Ymx7XGYwXGZuaWwgQXJpYWw7fX1ccGFyZFxmMFxmczIwIENDMCAtIERvbWluaW8gcHVibGljb1xwYXJ9</Data>
    </License>
    <Finish isRebootMachineEnabled="false">
      <Program executableFilePath="&lt;InstallPath&gt;\\CGEIHC.exe" isLaunchEnabled="true" arguments=""/>
    </Finish>
  </InstallerConfiguration>
  <UninstallerConfiguration isPackaged="true">
    <UninstallerExecutableFileName>Uninstall</UninstallerExecutableFileName>
    <OpenWebURIAfterUninstallation isEnabled="false">https://</OpenWebURIAfterUninstallation>
    <CustomDisplayIcon isEnabled="false" iconFilePath="&lt;InstallPath&gt;\\"/>
  </UninstallerConfiguration>
  <VisualUpdateConfiguration isPackaged="false">
    <ProductName>&lt;AppName&gt;</ProductName>
    <ProductVersion>&lt;AppVersion&gt;</ProductVersion>
    <Language>0</Language>
    <UpdateScriptURIs>
      <MainURI>https://</MainURI>
      <FirstMirrorURI>https://</FirstMirrorURI>
      <SecondMirrorURI>https://</SecondMirrorURI>
    </UpdateScriptURIs>
    <updateExecutableFileName>Update</updateExecutableFileName>
    <proEditionLicenseKey/>
    <Program executableFilePath="" isLaunchEnabled="false" isTerminationEnabled="false"/>
  </VisualUpdateConfiguration>
  <BuildConfiguration>
    <SetupFilePath>{SETUP_EXE}</SetupFilePath>
    <SetupIconPath/>
    <UninstallerIconPath/>
    <Compression method="0" level="2"/>
    <CodeSigning isEnabled="false" signToolCommandLine=""/>
  </BuildConfiguration>
</InstallForgeProject>
'''

with open(OUT_IFP, 'w', encoding='utf-8') as fw:
    fw.write(ifp_content)

total = len(root_files) + len(top_folders)
print(f'IFP generado: {OUT_IFP}')
print(f'Archivos raiz: {len(root_files)}, Carpetas: {len(top_folders)}')
print(f'Output: {SETUP_EXE}')
