@echo off
chcp 65001 >nul

echo =============================================
echo    LANCEMENT FINAL - CLOUD GAMING 3D
echo =============================================

:: --- 1. DEFINITION DES CHEMINS (SANS ESPACES) ---
set REPO_ROOT=%~dp0
if "%REPO_ROOT:~-1%"=="\" set REPO_ROOT=%REPO_ROOT:~0,-1%
set SERVER_EXE=%REPO_ROOT%\backend\out\build\x64-Debug\MonServeur.exe

:: --- 2. NETTOYAGE ---
echo [1/3] Nettoyage des anciens processus...
taskkill /f /im MonServeur.exe >nul 2>&1
taskkill /f /im python.exe >nul 2>&1

:: --- 3. VERIFICATION DE L'EXECUTABLE ---
if exist "%SERVER_EXE%" goto :LANCE_BACKEND

echo.
echo [ERREUR] Le moteur MonServeur.exe est absent.
echo Chemin cherche : %SERVER_EXE%
pause
exit /b

:LANCE_BACKEND
echo [2/3] Lancement du Moteur (Backend)...
start "Moteur_3D" "%SERVER_EXE%"

:LANCE_FRONTEND
echo [3/3] Lancement du Web (Frontend)...
:: On lance le serveur Python directement dans le bon dossier
start "Serveur_Web" /D "%REPO_ROOT%\frontend" python -m http.server 3000

:OUVERTURE
echo.
echo [OK] Tout est lance. Attente de 3 secondes...
timeout /t 3 /nobreak >nul
start http://localhost:3000

echo.
echo =============================================
echo    PROJET EN LIGNE !
echo =============================================
echo.
echo Fermez les fenetres noires pour arreter.
pause