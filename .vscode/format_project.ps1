$astyle_patterns = "*.c,*.cpp,*.h,*.hpp" # Define your file patterns here

# Get the current directory (project root)
$rootDir = $PSScriptRoot

# Find all directories, excluding the root itself and common hidden/build folders
Get-ChildItem -Path $rootDir -Directory -Recurse | Where-Object {
    $_.FullName -ne $rootDir -and
    $_.PSParentPath -notlike "*\\.vscode*" -and
    $_.PSParentPath -notlike "*\\build*" -and # Exclude a 'build' folder if you have one
    $_.PSParentPath -notlike "*\\bin*"    # Exclude a 'bin' folder if you have one
} | ForEach-Object {
    Write-Host "Formatting directory: $($_.FullName)"
    # Run AStyle in the current subdirectory using the project config
    # Using "." explicitly targets the current directory for recursive search within AStyle
    astyle --recursive --project "." "$astyle_patterns"
}

# Also format files directly in the root directory if needed
Write-Host "Formatting root directory: $rootDir"
astyle --project "$astyle_patterns"