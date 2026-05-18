#!/bin/bash
# Packaging script for IntelliEditor using Inno Setup from Git Bash.

set -e
cd "$(dirname "$0")"
if [ ! -f "../build/main.exe" ]; then
  echo "Erreur : ../build/main.exe introuvable. Compilez d'abord le projet."
  exit 1
fi

if ! command -v cmd.exe >/dev/null 2>&1; then
  echo "Erreur : cmd.exe introuvable. Exécutez ce script depuis Git Bash sous Windows."
  exit 1
fi

cmd.exe /C build_package.bat
