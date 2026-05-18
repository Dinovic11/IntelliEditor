@echo off
REM Packaging script for IntelliEditor using Inno Setup.
setlocal

REM Require built executables in ../build
if not exist "..\build\main.exe" (
    echo Erreur : build\main.exe introuvable. Compilez d'abord le projet.
    exit /b 1
)

set "ISCC="
if defined INNO_SETUP_PATH (
    if exist "%INNO_SETUP_PATH%\ISCC.exe" (
        set "ISCC=%INNO_SETUP_PATH%\ISCC.exe"
    ) else if exist "%INNO_SETUP_PATH%" (
        set "ISCC=%INNO_SETUP_PATH%"
    )
)
if not defined ISCC if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" set "ISCC=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if not defined ISCC if exist "%ProgramFiles%\Inno Setup 6\ISCC.exe" set "ISCC=%ProgramFiles%\Inno Setup 6\ISCC.exe"
if not defined ISCC (
    for /f "delims=" %%I in ('where ISCC.exe 2^>nul') do if not defined ISCC set "ISCC=%%~fI"
)

if not defined ISCC (
    echo Erreur : Inno Setup non trouvé. Installez Inno Setup 6 ou ajoutez ISCC.exe au PATH.
    echo Si Inno Setup est installé, définissez la variable d'environnement INNO_SETUP_PATH sur le dossier contenant ISCC.exe.
    exit /b 1
)

"%ISCC%" "IntelliEditor.iss"
if errorlevel 1 (
    echo Erreur : la génération du package a échoué.
    exit /b 1
)

echo Installateur généré avec succès dans ..\build\
endlocal
