@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul 2>&1

rem =============================================
rem  start.bat - Lancement one-click du projet
rem =============================================

rem Racine du depot = dossier de ce script
set REPO_ROOT=%~dp0
if "%REPO_ROOT:~-1%"=="\" set REPO_ROOT=%REPO_ROOT:~0,-1%

set SERVER_EXE=%REPO_ROOT%\backend\out\build\x64-Debug\MonServeur.exe

echo.
echo =============================================
echo   Cloud Gaming 3D - Demarrage
echo =============================================
echo.

rem --- 1. Verifier Python ---
where python >nul 2>&1
if errorlevel 1 (
    echo [Erreur] Python n'est pas installe ou pas dans le PATH.
    echo          Telechargez-le sur https://www.python.org/downloads/
    pause
    exit /b 1
)

rem --- 2. Compiler si necessaire ---
if not exist "%SERVER_EXE%" (
    echo [Build] MonServeur.exe introuvable. Lancement de la preparation...
    echo.
    powershell -ExecutionPolicy Bypass -NoProfile -File "%REPO_ROOT%\bootstrap_vcpkg.ps1"
    if not exist "%SERVER_EXE%" (
        echo.
        echo [Erreur] La compilation a echoue. Consultez les messages ci-dessus.
        pause
        exit /b 1
    )
    echo.
)

rem --- 3. Lancer le serveur backend ---
echo [OK] Lancement du serveur backend (MonServeur.exe)...
start "MonServeur" "%SERVER_EXE%"

rem --- 4. Lancer le serveur frontend ---
echo [OK] Lancement du serveur frontend (port 3000)...
start "FrontendServer" python -m http.server 3000 --directory "%REPO_ROOT%\frontend"

rem --- 5. Ouvrir le navigateur ---
echo [OK] Ouverture du navigateur dans 2 secondes...
timeout /t 2 /nobreak >nul
start http://localhost:3000

echo.
echo =============================================
echo   Tout est lance !
echo   - Backend  : localhost:8000 (WebSocket)
echo   - Frontend : localhost:3000 (Navigateur)
echo =============================================
echo.
echo Pour arreter : lancez stop.bat ou fermez cette fenetre.
pause
