# UG - Patch YSI y_hooks.inc
# Fixes the strict path check that fails with Open.mp pawncc 3.10.11
# This script is called automatically by compile.bat

$yhooks = Join-Path $PSScriptRoot "# open.mp\qawno\include\YSI\y_hooks.inc"

if (Test-Path $yhooks) {
    $content = [IO.File]::ReadAllText($yhooks)

    if ($content -match "#error Did you do") {
        # Replace the #error line with a comment
        $content = $content -replace '#error Did you do[^\r\n]*', '//#error PATCHED_BY_UG - path check disabled for pawncc 3.10.11'
        [IO.File]::WriteAllText($yhooks, $content)
        Write-Host "[PATCH] y_hooks.inc patched - removed strict path check"
    } else {
        Write-Host "[PATCH] y_hooks.inc already patched or no fix needed"
    }
} else {
    Write-Host "[PATCH] y_hooks.inc not found at: $yhooks"
}
