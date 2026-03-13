@echo off
echo [Demarrage] Lancement du serveur backend...
start "" "C:\Users\Utilisateur\source\repos\Projet_XLIM04_09-03\backend\out\build\x64-Debug\MonServeur.exe"

echo [Demarrage] Lancement du serveur frontend (port 3000)...
start "" python -m http.server 3000 --directory "%~dp0frontend"

echo [Demarrage] Ouverture du navigateur dans 2 secondes...
timeout /t 2 /nobreak >nul
start http://localhost:3000

echo.
echo Tout est lance. Ferme cette fenetre quand tu as fini.
echo Pour tout arreter : Ctrl+C dans les fenetres MonServeur et Python.
pause
