@echo off
setlocal enabledelayedexpansion
:: On reste en mode standard pour eviter les bugs d'encodage
chcp 850 >nul

echo =============================================
echo    Cloud Gaming 3D - Arret du projet
echo =============================================
echo.

:: 1. Arret du Backend (MonServeur.exe)
echo [1/2] Fermeture du Moteur 3D...
taskkill /F /IM MonServeur.exe >nul 2>&1
echo [OK] Tentative de fermeture de MonServeur terminee.

:: 2. Arret du Frontend (Port 3000)
echo [2/2] Fermeture du serveur Web (Port 3000)...

:: On cherche le PID du port 3000 et on le tue directement
:: On utilise une syntaxe lineaire pour eviter l'erreur "inattendu"
for /f "tokens=5" %%a in ('netstat -aon ^| findstr :3000 ^| findstr LISTENING') do (
    taskkill /F /PID %%a >nul 2>&1
)

:: 3. Securite : on tue les processus Python restants avec le bon titre
taskkill /F /IM python.exe /FI "WINDOWTITLE eq Frontend*" >nul 2>&1
taskkill /F /IM py.exe /FI "WINDOWTITLE eq Frontend*" >nul 2>&1

echo.
echo =============================================
echo    Tout est arrete (Port 3000 et Moteur).
echo =============================================
timeout /t 2 /nobreak >nul