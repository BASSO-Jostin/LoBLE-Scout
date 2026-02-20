# zephyr_env.ps1 - Configuration Zephyr pour votre machine
Write-Host "Configuration de Zephyr SDK..." -ForegroundColor Cyan

# 1. Chemin du SDK (déjà installé)
$SDK_PATH = "D:\Documents\M2\RTOS\zephyr-sdk-0.17.4"
$ZEPHYR_BASE = "D:\Git\zephyrprojet\v3.7\zephyr"

# 2. Définir les variables d'environnement
$env:ZEPHYR_BASE = $ZEPHYR_BASE
$env:ZEPHYR_SDK_INSTALL_DIR = $SDK_PATH
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"

# 3. Ajouter les bons chemins au PATH
$pathsToAdd = @(
    "$SDK_PATH\bin",           # Compilateur ARM
    "$ZEPHYR_BASE\scripts",    # Scripts Zephyr
    "D:\Disque_C\bin",         # CMake (déjà dans votre PATH)
    "C:\Python313\Scripts",    # Python
    "C:\Python313"
)

foreach ($path in $pathsToAdd) {
    if (Test-Path $path) {
        if ($env:PATH -notlike "*$path*") {
            $env:PATH = "$path;$env:PATH"
            Write-Host "✓ Ajouté au PATH : $path" -ForegroundColor Green
        }
    }
}

# 4. Exécuter le script d'environnement Zephyr
if (Test-Path "$ZEPHYR_BASE\zephyr-env.cmd") {
    Write-Host "Exécution de zephyr-env.cmd..." -ForegroundColor Yellow
    cmd /c "$ZEPHYR_BASE\zephyr-env.cmd" 'powershell'
} elseif (Test-Path "$ZEPHYR_BASE\zephyr-env.sh") {
    & "$ZEPHYR_BASE\zephyr-env.sh"
}

# 5. Vérifications
Write-Host "`n=== VÉRIFICATIONS ===" -ForegroundColor Cyan

# Test du compilateur
$compilerTest = Get-Command "arm-zephyr-eabi-gcc" -ErrorAction SilentlyContinue
if ($compilerTest) {
    Write-Host "✓ Compilateur trouvé : $($compilerTest.Source)" -ForegroundColor Green
    arm-zephyr-eabi-gcc --version
} else {
    Write-Host "✗ Compilateur NON trouvé" -ForegroundColor Red
    Write-Host "Vérifiez le dossier : $SDK_PATH\bin\" -ForegroundColor Yellow
    Write-Host "Fichiers présents :" -ForegroundColor Yellow
    Get-ChildItem "$SDK_PATH\bin\arm-zephyr-eabi-*" -ErrorAction SilentlyContinue | Select-Object Name
}

# Vérifier west
$westTest = Get-Command "west" -ErrorAction SilentlyContinue
if ($westTest) {
    Write-Host "✓ West trouvé : $($westTest.Source)" -ForegroundColor Green
} else {
    Write-Host "✗ West non trouvé" -ForegroundColor Red
}

# Variables d'environnement
Write-Host "`nVariables d'environnement :" -ForegroundColor Cyan
Write-Host "ZEPHYR_BASE: $env:ZEPHYR_BASE"
Write-Host "ZEPHYR_SDK_INSTALL_DIR: $env:ZEPHYR_SDK_INSTALL_DIR"
Write-Host "ZEPHYR_TOOLCHAIN_VARIANT: $env:ZEPHYR_TOOLCHAIN_VARIANT"

Write-Host "`n=== PRÊT À UTILISER ===" -ForegroundColor Green
Write-Host "Pour builder votre projet :" -ForegroundColor Yellow
Write-Host "cd D:\Git\zephyrprojet\v3.7\Mon_Apk" -ForegroundColor White
Write-Host "west build -b rak11720 -- -DBOARD_ROOT=`"..\..`"" -ForegroundColor White