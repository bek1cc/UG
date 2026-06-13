# ============================================================
# UG Launcher - Generate Manifest & Copy Models
# Pokreni ovu skriptu u PowerShell-u iz root foldera repo-a
# ============================================================

$ErrorActionPreference = "Stop"

# Podesi ove putanje
$AnimalsSource = "D:\Animals"
$ModelsDir = ".\Models"
$ManifestPath = ".\Models\manifest.json"
$LauncherManifest = ".\Launcher\files_manifest.json"

Write-Host ""
Write-Host "============================================" -ForegroundColor Red
Write-Host "  UG Launcher - Manifest Generator" -ForegroundColor Red
Write-Host "============================================" -ForegroundColor Red
Write-Host ""

# Provjeri da li Animals folder postoji
if (-not (Test-Path $AnimalsSource)) {
    Write-Host "[X] Folder $AnimalsSource ne postoji!" -ForegroundColor Red
    Write-Host "    Promijeni $AnimalsSource varijablu u skripti." -ForegroundColor Yellow
    exit 1
}

# Kreiraj Models folder ako ne postoji
if (-not (Test-Path $ModelsDir)) {
    New-Item -ItemType Directory -Path $ModelsDir | Out-Null
    Write-Host "[+] Kreiran folder: $ModelsDir" -ForegroundColor Green
}

# Pronadji sve .dff i .txd fajlove
$extensions = @("*.dff", "*.txd")
$allFiles = @()

foreach ($ext in $extensions) {
    $found = Get-ChildItem -Path $AnimalsSource -Filter $ext -Recurse -File
    $allFiles += $found
}

if ($allFiles.Count -eq 0) {
    Write-Host "[X] Nema .dff/.txd fajlova u $AnimalsSource!" -ForegroundColor Red
    exit 1
}

Write-Host "[+] Pronadjeno $($allFiles.Count) fajlova" -ForegroundColor Cyan
Write-Host ""

# Kopiraj fajlove u Models/ folder
$copied = 0
foreach ($file in $allFiles) {
    $dest = Join-Path $ModelsDir $file.Name
    
    # Preskoci ako vec postoji i isti je
    if (Test-Path $dest) {
        $existingHash = (Get-FileHash -Path $dest -Algorithm MD5).Hash
        $sourceHash = (Get-FileHash -Path $file.FullName -Algorithm MD5).Hash
        if ($existingHash -eq $sourceHash) {
            Write-Host "  [SKIP] $($file.Name) (vec postoji)" -ForegroundColor DarkGray
            continue
        }
    }
    
    Copy-Item -Path $file.FullName -Destination $dest -Force
    $copied++
    Write-Host "  [COPY] $($file.Name)" -ForegroundColor Green
}

Write-Host ""
Write-Host "[+] Kopirano: $copied, Preskoceno: $($allFiles.Count - $copied)" -ForegroundColor Cyan

# Generisi manifest
$baseUrl = "https://raw.githubusercontent.com/bek1cc/UG/main/Models/"
$serverIp = "217.156.22.164:7774"

$manifestFiles = @()

$modelFiles = Get-ChildItem -Path $ModelsDir -Include "*.dff","*.txd" -Recurse -File

$total = $modelFiles.Count
$current = 0

foreach ($mf in $modelFiles) {
    $current++
    Write-Progress -Activity "Racunanje MD5 hashova" -Status "$current / $total" -PercentComplete (($current / $total) * 100)
    
    $hash = (Get-FileHash -Path $mf.FullName -Algorithm MD5).Hash.ToLower()
    $relativePath = $mf.Name
    
    # Odredi localPath u GTA folderu
    $ext = $mf.Extension.ToLower()
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($mf.Name)
    $localPath = "SAMP\cache\models\$baseName$ext"
    
    $manifestFiles += @{
        localPath = $localPath
        url = "$baseUrl$relativePath"
        hash = $hash
    }
}

Write-Progress -Activity "Racunanje MD5 hashova" -Completed

# Kreiraj JSON
$manifest = @{
    baseUrl = $baseUrl
    serverIp = $serverIp
    files = $manifestFiles
} | ConvertTo-Json -Depth 5

# Sacuvaj Models manifest
$manifest | Out-File -FilePath $ManifestPath -Encoding UTF8
Write-Host "[+] Sacuvan: $ManifestPath" -ForegroundColor Green

Write-Host ""
Write-Host "Gotovo! Manifest ima $($manifestFiles.Count) fajlova." -ForegroundColor Green
Write-Host ""
Write-Host "Slijedeci koraci:" -ForegroundColor Yellow
Write-Host "  1. git add ." -ForegroundColor White
Write-Host "  2. git commit -m 'Add animal models + manifest'" -ForegroundColor White
Write-Host "  3. git push origin main" -ForegroundColor White
Write-Host ""
