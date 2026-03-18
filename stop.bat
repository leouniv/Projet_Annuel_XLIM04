@echo off
echo.
echo =============================================
echo   Cloud Gaming 3D - Arret
echo =============================================
echo.
echo [Arret] Fermeture de MonServeur.exe...
taskkill /F /IM MonServeur.exe >nul 2>&1
echo [Arret] Fermeture du serveur Python...
taskkill /F /FI "WINDOWTITLE eq FrontendServer*" >nul 2>&1
taskkill /F /IM python.exe >nul 2>&1
echo.
echo [OK] Tout est arrete.
timeout /t 2 /nobreak >nul
