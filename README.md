# Cloud Gaming 3D - Cube Demo

Rendu 3D côté serveur (C++/OpenGL) streamé vers un navigateur via WebRTC.

## Prérequis

| Outil | Téléchargement |
|---|---|
| **Visual Studio Build Tools** (ou VS avec charge C++) | [aka.ms/vs/buildtools](https://aka.ms/vs/17/release/vs_BuildTools.exe) |
| **CMake** ≥ 3.10 | [cmake.org/download](https://cmake.org/download/) |
| **Python** ≥ 3.8 | [python.org/downloads](https://www.python.org/downloads/) |
| **Ninja** | Inclus avec Visual Studio, sinon [github.com/ninja-build](https://github.com/ninja-build/ninja/releases) |

> **Note :** vcpkg et toutes les dépendances C++ sont téléchargés **automatiquement** au premier lancement.

## Démarrage rapide (sans IDE)

```
git clone https://github.com/leouniv/Projet_Annuel_XLIM04.git
cd Projet_Annuel_XLIM04
start.bat
```

C'est tout. Le script :
1. Télécharge **vcpkg** si absent
2. Installe les dépendances C++ (`libdatachannel`, `glfw3`, `glm`, `nlohmann-json`, `libjpeg-turbo`)
3. Compile le backend
4. Lance le serveur backend (port 8000) et le frontend (port 3000)
5. Ouvre le navigateur

## Arrêt

```
stop.bat
```

## Structure du projet

```
├── backend/
│   ├── CMakeLists.txt          # Build system
│   ├── CMakePresets.json       # Presets CMake (IDE-agnostique)
│   ├── CMakeSettings.json      # Config Visual Studio
│   ├── vcpkg.json              # Dépendances C++
│   ├── main.cpp                # Serveur WebRTC + game loop
│   ├── Renderer.cpp/.hpp       # Moteur de rendu OpenGL
├── frontend/
│   ├── index.html              # Interface navigateur
│   ├── script.js               # Client WebRTC
├── bootstrap_vcpkg.ps1         # Script d'installation automatique
├── start.bat                   # Lancement one-click
├── stop.bat                    # Arrêt propre
```

## Utilisation avec un IDE

### Visual Studio
Ouvrir le dossier `backend/` → VS détecte `CMakeSettings.json` automatiquement.

### VS Code / CLion / autre
Les presets CMake standard (`CMakePresets.json`) sont utilisés automatiquement.

```
cd backend
cmake --preset default
cmake --build --preset default
```
