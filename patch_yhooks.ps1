# UG - Patch ALL YSI include files
# Fixes the strict path checks that fail with Open.mp pawncc 3.10.11
# IMPORTANT: Must preserve original file encoding (ASCII/ANSI) 
# PAWN compiler cannot read UTF-8 with BOM

$ysiDir = Join-Path $PSScriptRoot "# open.mp\qawno\include\YSI"

if (-not (Test-Path $ysiDir)) {
    Write-Host "[PATCH] YSI directory not found: $ysiDir"
    exit 0
}

$patchedCount = 0

# Find ALL .inc files in YSI directory and subdirectories
$incFiles = Get-ChildItem -Path $ysiDir -Filter "*.inc" -Recurse

foreach ($file in $incFiles) {
    # Read as RAW bytes to detect encoding
    $bytes = [IO.File]::ReadAllBytes($file.FullName)
    
    # Convert to string using default encoding (preserves original)
    $content = [System.Text.Encoding]::Default.GetString($bytes)
    
    if ($content -match "Did you do") {
        # Replace ALL lines containing "Did you do" #error
        # Match any line with #error that contains "Did you do"
        $newContent = $content -replace '(?m)^(\s*)#error\s+Did you do[^\r\n]*', '$1//#error PATCHED_BY_UG'
        
        # Write back using DEFAULT encoding (Windows-1252/ANSI) - NO BOM
        $newBytes = [System.Text.Encoding]::Default.GetBytes($newContent)
        [IO.File]::WriteAllBytes($file.FullName, $newBytes)
        Write-Host "[PATCH] Patched: $($file.Name)"
        $patchedCount++
    }
}

if ($patchedCount -eq 0) {
    Write-Host "[PATCH] No YSI files need patching"
} else {
    Write-Host "[PATCH] Patched $patchedCount YSI file(s)"
}
