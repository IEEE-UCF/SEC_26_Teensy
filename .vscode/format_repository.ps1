# .vscode/format_repository.ps1

# Navigate to the repository root (where .vscode is located)
# This helps ensure the script runs relative to the repo root
$ScriptDir = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition
Set-Location -Path $ScriptDir\..

Write-Host "Starting clang-format on the repository..."

# Define the file extensions to format
$Extensions = "*.cpp", "*.h", "*.c", "*.hpp", "*.hxx", "*.cxx", "*.ino"

# Find files recursively and format them in place
Get-ChildItem -Path . -Recurse -Include $Extensions | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "Formatting: $filePath"
    try {
        clang-format -i $filePath
    } catch {
        # Corrected line: Use $().Exception.Message or ${_.Exception.Message}
        Write-Error "Error formatting"
        # Or using ${}:
        # Write-Error "Error formatting $filePath: ${_.Exception.Message}"
    }
}

Write-Host "Clang-format operation finished."