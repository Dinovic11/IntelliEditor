# Configuration
BUILD_DIR = build
CMAKE = cmake
EXE = $(BUILD_DIR)/main.exe

.PHONY: all build run clean help

# compile et lance
all: build run

# Création du dossier build et compilation
build:
	@$(CMAKE) -E make_directory $(BUILD_DIR)
	@$(CMAKE) -B $(BUILD_DIR) -G "MinGW Makefiles"
	@$(CMAKE) --build $(BUILD_DIR)

# Exécution du programme
run:
	@echo ---------------------------------------
	@./$(EXE)

# Nettoyage
clean:
	@$(CMAKE) -E remove_directory $(BUILD_DIR)
	@echo Dossier build supprime.

# Aide
help:
	@echo "Commandes disponibles :"
	@echo "  make build  : Compile le projet"
	@echo "  make run    : Lance l'executable"
	@echo "  make all    : Compile et lance (par defaut)"
	@echo "  make clean  : Supprime les fichiers de compilation"
