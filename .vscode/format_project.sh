#!/bin/bash

astyle_patterns="*.c,*.cpp,*.h,*.hpp" # Define your file patterns here

# Get the directory of the script (project root)
rootDir="$(dirname "$0")"
cd "$rootDir"

echo "Formatting root directory: $(pwd)"
# Format files directly in the root directory
astyle --project "$astyle_patterns"

# Find all subdirectories, excluding common hidden/build folders
find . -type d \( -name .vscode -o -name build -o -name bin \) -prune -o -print | while read -r dir; do
    if [ "$dir" != "." ]; then
        echo "Formatting directory: $dir"
        # Change to the subdirectory and run AStyle recursively from there
        (cd "$dir" && astyle --recursive --project "." "$astyle_patterns")
    fi
done