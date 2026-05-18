[Setup]
AppName=IntelliEditor
AppVersion=1.0
DefaultDirName={pf}\IntelliEditor
DefaultGroupName=IntelliEditor
OutputDir=..\build
OutputBaseFilename=IntelliEditor-Setup
Compression=lzma
SolidCompression=yes
DisableWelcomePage=yes
DisableDirPage=no
DisableProgramGroupPage=no
DisableStartupPrompt=yes
UninstallDisplayIcon={app}\IntelliEditor.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "Créer une icône sur le Bureau"; GroupDescription: "Tâches supplémentaires :"; Flags: unchecked

[Files]
Source: "..\build\main.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\win32_demo.exe"; DestDir: "{app}"; DestName: "IntelliEditor.exe"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\config.ini"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\IntelliEditor"; Filename: "{app}\IntelliEditor.exe"
Name: "{group}\Désinstaller IntelliEditor"; Filename: "{uninstallexe}"
Name: "{commondesktop}\IntelliEditor"; Filename: "{app}\IntelliEditor.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\IntelliEditor.exe"; Description: "Lancer IntelliEditor"; Flags: nowait postinstall skipifsilent
