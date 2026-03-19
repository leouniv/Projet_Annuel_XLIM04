@echo off
echo [1/2] Arret des anciens serveurs Python...
taskkill /f /im python.exe >nul 2>&1

echo [2/2] Lancement du serveur Web dans le dossier Frontend...
:: On se deplace PHYSIQUEMENT dans le dossier avant de lancer Python
cd /d "%~dp0\frontend"
start "Serveur_Web" python -m http.server 3000

timeout /t 2
echo [OK] Ouverture du navigateur...
start http://localhost:3000