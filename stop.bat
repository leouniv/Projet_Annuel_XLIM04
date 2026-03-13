@echo off
echo [Arret] Fermeture de MonServeur.exe...
taskkill /F /IM MonServeur.exe >nul 2>&1
echo [Arret] Fermeture de Python...
taskkill /F /IM python.exe >nul 2>&1
echo Tout est arrete.
timeout /t 2 /nobreak >nul
