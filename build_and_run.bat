@echo off
rem Script pour compiler et exécuter le projet Rendu3D_Server.
rem IMPORTANT : Ce script DOIT être exécuté depuis une "Invite de commandes Développeur pour Visual Studio".

echo --- Début du script de build et d'exécution ---

rem --- Configuration des chemins ---
set "SCRIPT_DIR=%~dp0"
set "TOOLCHAIN_FILE=%SCRIPT_DIR%vcpkg\scripts\buildsystems\vcpkg.cmake"
rem On utilise un dossier de build dedié pour Visual Studio pour ne pas
rem entrer en conflit avec un eventuel build "Ninja" de VS Code.
set "BUILD_DIR=%SCRIPT_DIR%build_vs"
set "SOURCE_DIR=%SCRIPT_DIR%backend"
set "EXECUTABLE_NAME=MonServeur.exe"

rem --- Configuration de l'environnement Visual Studio ---
echo [PRE-STEP] Configuration de l'environnement Visual Studio...

rem Si on est DEJA dans un terminal Visual Studio (Developer Command Prompt / PowerShell),
rem VSCMD_VER est défini et l'environnement est prêt : on ne rappelle pas vcvarsall.bat.
if defined VSCMD_VER (
    echo Environnement Visual Studio deja configure (^VSCMD_VER=%VSCMD_VER%^). 
) else (
    rem Sinon, on essaie plusieurs versions (2026, 2022, 2019). Adapte si besoin.
    if exist "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else if exist "C:\Program Files\Microsoft Visual Studio\17\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\17\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else (
        echo ERREUR : Impossible de trouver vcvarsall.bat. Lance ce script depuis l'"Invite de commandes developpeur" ou mets a jour le chemin dans build_and_run.bat.
        pause
        exit /b 1
    )
)

rem --- Étape 1: Configuration du projet ---
echo [1/3] Configuration du projet avec CMake...

rem IMPORTANT :
rem - Adapte le generateur CMake a ta version de Visual Studio :
rem   * VS 2026 : "Visual Studio 18 2026"
rem   * VS 2022 : "Visual Studio 17 2022"
rem   * VS 2019 : "Visual Studio 16 2019"
rem - Si tu utilises l'Invite de commandes developpeur, le parametre -A x64 peut etre omis.

call cmake -G "Visual Studio 18 2026" -A x64 -B "%BUILD_DIR%" -S "%SOURCE_DIR%" "-DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE%"
if %errorlevel% neq 0 (
    echo ERREUR : La configuration CMake a échoué.
    exit /b 1
)
echo Configuration terminée avec succès.

rem --- Étape 2: Compilation du projet ---
echo [2/3] Compilation du projet...
call cmake --build "%BUILD_DIR%"
if %errorlevel% neq 0 (
    echo ERREUR : La compilation a échoué.
    exit /b 1
)
echo Compilation terminée avec succès.

rem --- Étape 3: Exécution du programme ---
echo [3/3] Lancement de l'application...
set "EXE_PATH=%BUILD_DIR%\Debug\%EXECUTABLE_NAME%"
if not exist "%EXE_PATH%" (
    set "EXE_PATH=%BUILD_DIR%\%EXECUTABLE_NAME%"
)

if exist "%EXE_PATH%" (
    call "%EXE_PATH%"
) else (
    echo ERREUR : L'exécutable n'a pas été trouvé. Emplacements vérifiés : %BUILD_DIR%\Debug\ et %BUILD_DIR%\
    exit /b 1
)

echo --- Script terminé ---