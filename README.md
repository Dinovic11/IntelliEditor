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

### Test NLP / LLM

Un exemple de flux NLP est également disponible via `main.exe` avec l'option `--nlp-test`.
Cette option appelle `nlp_init()`, tente `nlp_process_text()` et bascule proprement en mode autonome si aucun modèle n'est disponible.

```powershell
cd /c/Users/paisible/Work/Web/PERSO/IntelliEditor
cmake --build build --config Release
./build/main.exe --nlp-test
```

Le stub `src/nlp_stub.c` est prévu pour être remplacé par la vraie implémentation LLM de Dev C.

