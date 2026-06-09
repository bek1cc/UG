# UG - Patch ALL YSI include files
# Fixes the strict path checks that fail with Open.mp pawncc 3.10.11
# YSI uses #error checks for forward slashes vs backslashes
# pawncc 3.10.11 resolves paths differently, triggering these errors
# This script patches ALL YSI .inc files at once

$ysiDir = Join-Path $PSScriptRoot "# open.mp\qawno\include\YSI"

if (-not (Test-Path $ysiDir)) {
    Write-Host "[PATCH] YSI directory not found: $ysiDir"
    exit 0
}

$patchedCount = 0

# Find ALL .inc files in YSI directory and subdirectories
$incFiles = Get-ChildItem -Path $ysiDir -Filter "*.inc" -Recurse

foreach ($file in $incFiles) {
    $content = [IO.File]::ReadAllText($file.FullName)
    
    if ($content -match "#error Did you do") {
        # Replace ALL #error lines that contain "Did you do" with comments
        $newContent = $content -replace '(?m)^#error Did you do[^\r\n]*', '//#error PATCHED_BY_UG - path check disabled for pawncc 3.10.11'
        [IO.File]::WriteAllText($file.FullName, $newContent)
        Write-Host "[PATCH] Patched: $($file.Name)"
        $patchedCount++
    }
}

if ($patchedCount -eq 0) {
    Write-Host "[PATCH] No YSI files need patching (already patched or no checks found)"
} else {
    Write-Host "[PATCH] Patched $patchedCount YSI file(s)"
}
