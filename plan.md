# Plan étape par étape pour DEV-A (Infrastructure & Éditeur de texte)

## Contexte

D’après le document, DEV-A est responsable des fondations techniques du projet et du moteur d’édition de texte. La répartition des tâches place DEV-A sur :

- Infrastructure C / build
- Gap buffer
- Undo/redo
- Encodage UTF-8/UTF-16
- Configuration INI
- Mise en forme du texte
- Export / recherche
- Optimisation mémoire
- Packaging

## Phase 1 — Fondations et noyau de l’éditeur

### Mise en place de l’environnement

- Configurer CMake
- Installer / valider MSYS2 + MinGW-w64
- S’assurer que la toolchain compile en C11 sur Windows

### Implémenter le cœur du moteur d’édition

#### Module editor/gap_buffer

- Structure de données gap buffer pour insertions/suppressions rapides
- API claire pour l’éditeur, séparée du reste du système

#### Ajouter l’historique

#### Module editor/undo_redo

- Pattern Command pour undo/redo illimité

#### Gérer l’encodage

#### Module utils/encoding

- Pipeline interne en UTF-8
- Conversion vers UTF-16 uniquement aux points d’entrée/sortie Win32

#### Gérer la configuration

#### Module utils/config

- Lecture/écriture de config.ini
- Stockage sous %APPDATA%\IntelliEditor\config.ini

## Phase 2 — Fonctionnalités avancées de l’éditeur

### Mise en forme et styles

#### Module editor/formatter

Support :

- gras, italique, souligné
- titres H1..H4
- listes à puces / numérotées

### Export de documents

#### Module editor/exporter

Implémenter :

- .txt UTF-8
- .rtf (génération manuelle des balises)
- format propriétaire .ie

### Recherche / remplacement

#### Module editor/search_replace

- Recherche texte, remplacement, support regex optionnel

### Statistiques de base

- Nombre de mots, caractères, paragraphes en temps réel
- Bien que lié à l’éditeur, c’est un point attendu dans la fonctionnalité de base

## Phase 3 — Stabilisation et optimisation

### Optimisation mémoire

#### Module utils/memory

- Allocateur/debug mémoire
- Prévoir détection de fuites et usage efficace

### Préparer l’éditeur en mode “base autonome”

- S’assurer que l’éditeur fonctionne si le LLM ou la couche NLP est indisponible
- Séparer proprement l’API éditeur de l’UI Win32 (DEV-B) et du NLP/LLM (DEV-C)

## Phase 4 — Packaging et livraison

### Préparer l’installation

- Dossier installer/
- Script Inno Setup pour générer l’installeur Windows

### Documenter la compilation

- README / instructions build
- Inclure la configuration du projet pour DEV-A

## Priorités à suivre strictement

- Respecter la séparation des couches : editor doit exposer une API C claire et ne pas dépendre de l’UI Win32.
- Tout le pipeline interne doit être en UTF-8.
- Le build doit être reproductible avec CMake + MinGW-w64.
- Les tâches de DEV-A sont clairement listées au document :
  - editor/gap_buffer
  - editor/undo_redo
  - utils/encoding
  - utils/config
  - editor/formatter
  - editor/exporter
  - editor/search_replace
  - utils/memory
  - installer/

## Résumé du plan par phase

- Phase 1 : fondations techniques + gap buffer + undo/redo + encodage + config
- Phase 2 : styles/formatage + export (.txt, .rtf, .ie) + recherche/remplacement
- Phase 3 : optimisation mémoire + robustesse du moteur d’édition
- Phase 4 : packaging Windows avec Inno Setup + documentation build