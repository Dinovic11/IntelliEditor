# IntelliEditor
Logiciel de traitement de texte intelligent en C (Projet C Avancé), fonctionnant entièrement hors ligne sous Windows.

## Contexte
Projet académique – C Avancé (L3 Génie Logiciel)  
Université Don Bosco de Lubumbashi – Année académique 2025–2026

## Fonctionnalités principales
- Éditeur de texte riche
- Correction orthographique et grammaticale française
- Moteur de règles de rédaction basé sur JSON
- Assistance intelligente hors ligne (LLM embarqué)

## Équipe
- DEV-A : SANGWA MBALE Sarah
- DEV-B : KAPOYA MUTUNDUKA Barley
- DEV-C : KAPINGA MBALA Rholsel
- DEV-D : KAMBALE KULE VALI HALI Dinovic

## Compilation
(à compléter)

## Licence
(à définir)

## Prototype Win32 (Dev B)

Un prototype Win32 minimal est fourni pour démontrer l'utilisation de l'API de l'éditeur (`editor_api`). Il ouvre une fenêtre avec un contrôle d'édition multiligne et synchronise son contenu avec le noyau éditeur.

Pour compiler et lancer le prototype :

```powershell
cd /c/Users/paisible/Work/Web/PERSO/IntelliEditor
cmake --build build --config Release
./build/win32_demo.exe
```

Le prototype est volontairement simple : il montre comment Dev B peut intégrer le `editor_api` sans connaître les détails internes du buffer.

> Note : le binaire `main.exe` est un test en ligne de commande et se termine immédiatement. Pour l’interface graphique installée, utilisez `win32_demo.exe` en local ou l’application installée `IntelliEditor.exe`.

### Test NLP / LLM

Un exemple de flux NLP est également disponible via `main.exe` avec l'option `--nlp-test`.
Cette option appelle `nlp_init()`, tente `nlp_process_text()` et bascule proprement en mode autonome si aucun modèle n'est disponible.

```powershell
cd /c/Users/paisible/Work/Web/PERSO/IntelliEditor
cmake --build build --config Release
./build/main.exe --nlp-test
```

Le stub `src/nlp_stub.c` est prévu pour être remplacé par la vraie implémentation LLM de Dev C.

## Packaging

Le dossier `installer/` contient le script Inno Setup `IntelliEditor.iss` et un script de packaging `build_package.bat`.

Pour générer l’installateur, installez Inno Setup 6 et lancez l’un des deux scripts.

- Depuis PowerShell ou CMD :

```powershell
cd C:\Users\paisible\Work\Web\PERSO\IntelliEditor\installer
.\build_package.bat
```

- Depuis Git Bash :

```bash
cd /c/Users/paisible/Work/Web/PERSO/IntelliEditor/installer
./build_package.sh
```

- Depuis la racine du projet avec CMake :

```powershell
cd C:\Users\paisible\Work\Web\PERSO\IntelliEditor
cmake --build build --target package
```

Si Inno Setup est installé mais n’est pas trouvé automatiquement, définissez la variable d’environnement `INNO_SETUP_PATH` vers le dossier contenant `ISCC.exe` :

```powershell
setx INNO_SETUP_PATH "C:\Program Files (x86)\Inno Setup 6"
```

Si vous exécutez `build_package.bat` manuellement dans CMD avec un chemin contenant des espaces, utilisez cette syntaxe :

```cmd
set "INNO_SETUP_PATH=C:\Program Files (x86)\Inno Setup 6"
build_package.bat
```

Puis redémarrez votre terminal si vous avez utilisé `setx`.

```

L’installateur généré sera placé dans le répertoire `build/`.

