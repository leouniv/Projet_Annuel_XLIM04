# =============================================================================
# bootstrap_vcpkg.ps1
# Telecharge vcpkg, installe les dependances et compile le backend.
# Usage : Executer depuis la racine du depot (auto-detectee).
# =============================================================================

$ErrorActionPreference = 'Stop'
$repoRoot   = Split-Path -Parent $MyInvocation.MyCommand.Path
$vcpkgDir   = Join-Path $repoRoot 'vcpkg'
$backendDir = Join-Path $repoRoot 'backend'
$buildDir   = Join-Path $backendDir 'out\build\x64-Debug'

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "   Preparation automatique du projet"        -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Racine du depot : $repoRoot"
Write-Host ""

# -----------------------------------------------------------------------------
# 1. Verification des prerequis
# -----------------------------------------------------------------------------
Write-Host "[1/5] Verification des prerequis..." -ForegroundColor Yellow

$missing = @()

# CMake
if (-Not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    $missing += "CMake (https://cmake.org/download/)"
}

# Compilateur C++ (cl.exe via Visual Studio Build Tools ou VS)
$hasCompiler = $false
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -property installationPath 2>$null
    if ($vsPath) { $hasCompiler = $true }
}
if (-Not $hasCompiler -and -Not (Get-Command cl -ErrorAction SilentlyContinue)) {
    $missing += "Compilateur C++ MSVC (Visual Studio Build Tools ou Visual Studio avec charge de travail C++)"
}

# Python (pour le serveur frontend)
if (-Not (Get-Command python -ErrorAction SilentlyContinue)) {
    $missing += "Python 3 (https://www.python.org/downloads/)"
}

if ($missing.Count -gt 0) {
    Write-Host ""
    Write-Host "  PREREQUIS MANQUANTS :" -ForegroundColor Red
    foreach ($m in $missing) {
        Write-Host "    - $m" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "  Installez-les puis relancez ce script." -ForegroundColor Red
    Write-Host ""
    exit 1
}

Write-Host "  Tous les prerequis sont presents." -ForegroundColor Green

# -----------------------------------------------------------------------------
# 2. Telecharger vcpkg si absent
# -----------------------------------------------------------------------------
Write-Host "[2/5] Preparation de vcpkg..." -ForegroundColor Yellow

if (-Not (Test-Path $vcpkgDir)) {
    Write-Host "  vcpkg introuvable. Telechargement via ZIP..."
    $zipUrl  = 'https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip'
    $tmpZip  = Join-Path $env:TEMP 'vcpkg-master.zip'

    Invoke-WebRequest -Uri $zipUrl -OutFile $tmpZip -UseBasicParsing

    $tmpExtract = Join-Path $env:TEMP 'vcpkg-extract'
    if (Test-Path $tmpExtract) { Remove-Item -Recurse -Force $tmpExtract }
    Expand-Archive -Path $tmpZip -DestinationPath $tmpExtract -Force

    # L'archive contient un dossier vcpkg-master
    $inner = Get-ChildItem $tmpExtract -Directory | Select-Object -First 1
    if (-Not $inner) {
        Write-Error "Echec de l'extraction de l'archive vcpkg."
        exit 1
    }

    Move-Item -Force $inner.FullName $vcpkgDir
    Remove-Item -Recurse -Force $tmpExtract
    Remove-Item -Force $tmpZip
    Write-Host "  vcpkg telecharge dans $vcpkgDir" -ForegroundColor Green
} else {
    Write-Host "  vcpkg deja present." -ForegroundColor Green
}

# -----------------------------------------------------------------------------
# 3. Bootstrap vcpkg
# -----------------------------------------------------------------------------
Write-Host "[3/5] Bootstrap de vcpkg..." -ForegroundColor Yellow

$vcpkgExe = Join-Path $vcpkgDir 'vcpkg.exe'
if (-Not (Test-Path $vcpkgExe)) {
    $bootstrap = Join-Path $vcpkgDir 'bootstrap-vcpkg.bat'
    if (-Not (Test-Path $bootstrap)) {
        Write-Error "bootstrap-vcpkg.bat introuvable dans $vcpkgDir"
        exit 1
    }
    Push-Location $vcpkgDir
    & cmd.exe /c "bootstrap-vcpkg.bat -disableMetrics"
    Pop-Location

    if (-Not (Test-Path $vcpkgExe)) {
        Write-Error "vcpkg.exe introuvable apres bootstrap."
        exit 1
    }
}
Write-Host "  vcpkg.exe pret." -ForegroundColor Green

# -----------------------------------------------------------------------------
# 4. Configurer avec CMake (utilise CMakePresets.json)
# -----------------------------------------------------------------------------
Write-Host "[4/5] Configuration CMake (preset 'default')..." -ForegroundColor Yellow

if (-Not (Test-Path $buildDir)) { New-Item -ItemType Directory -Force -Path $buildDir | Out-Null }

Push-Location $backendDir
cmake --preset default
if ($LASTEXITCODE -ne 0) {
    Pop-Location
    Write-Error "Echec de la configuration CMake."
    exit 1
}
Pop-Location
Write-Host "  Configuration terminee." -ForegroundColor Green

# -----------------------------------------------------------------------------
# 5. Compiler
# -----------------------------------------------------------------------------
Write-Host "[5/5] Compilation..." -ForegroundColor Yellow

Push-Location $backendDir
cmake --build --preset default
if ($LASTEXITCODE -ne 0) {
    Pop-Location
    Write-Error "Echec de la compilation."
    exit 1
}
Pop-Location

Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "   Build termine avec succes !"              -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host "Lancez start.bat pour demarrer le projet."
Write-Host ""
