#!/bin/bash
# Copyright (c) 2013-2016 The Bitcoin Core developers
# Copyright (c) 2017-2019 The Raven Core developers
# Copyright (c) 2025 The Tenzura Core developers
# Copyright (c) 2025 The Tenzura Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# Ravencoin to Tenzura Rebrand Script - 2025-05-05
# This script preserves compilation integrity while rebranding

set -e  # Exit on error

echo "Starting Ravencoin to Tenzura rebranding..."

# Create backup
echo "Creating backup..."
BACKUP_DIR="../ravencoin-backup-$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"
cp -r . "$BACKUP_DIR"
echo "Backup created at $BACKUP_DIR"

# STEP 1: Handle special files that need careful treatment
echo "Handling special files first..."
if [ -f "configure.ac" ]; then
    sed -i 's/\bAC_INIT.*ravencoin/AC_INIT([tenzura], [VERSION], [https:\/\/github.com\/tenzura\/tenzura\/issues])/' configure.ac
    sed -i 's/\braven/tenzura/g; s/\bRaven/Tenzura/g; s/\bRAVEN/TENZURA/g' configure.ac
fi

# STEP 2: Process all .h files to update include guards
echo "Updating header guards..."
find . -name "*.h" | xargs grep -l "TENZURA" | while read file; do
    # Skip binary files
    if file "$file" | grep -q "binary"; then
        continue
    fi
    
    # Update header guards but preserve other code
    sed -i 's/\(#ifndef\s\+\)RAVEN_/\1TENZURA_/g; s/\(#define\s\+\)RAVEN_/\1TENZURA_/g' "$file"
done

# STEP 3: Add copyright and process content
echo "Processing file content..."
find . -type f -not -path "*/\.git/*" -not -path "*/\.*" | while read file; do
    # Skip binary files
    if file "$file" | grep -q "binary" || [[ "$file" == *.png ]] || [[ "$file" == *.ico ]] || [[ "$file" == *.jpg ]]; then
        continue
    fi
    
    # Add copyright after Tenzura copyright if it exists
    if grep -q "Copyright.*Raven Core developers" "$file"; then
# Copyright (c) 2025 The Tenzura Core developers
        sed -i '/Copyright.*Raven Core developers/a # Copyright (c) 2025 The Tenzura Core developers' "$file"
# Copyright (c) 2025 The Tenzura Core developers
    fi
    
    # Replace content except in copyright lines
    sed -i '/Copyright/!s/\braven\b/tenzura/g; /Copyright/!s/\bRaven\b/Tenzura/g; /Copyright/!s/\bRAVEN\b/TENZURA/g' "$file"
    
    # Fix config includes that might have special formatting
    sed -i 's/config\/tenzura-config/config\/tenzura-config/g' "$file"
    
    # Fix namespace declarations
    sed -i 's/namespace tenzura/namespace tenzura/g' "$file"
    
    # Fix class names with careful word boundaries
    sed -i 's/\bCRaven/CTenzura/g; s/\bCraven/CTenzura/g' "$file"
done

# STEP 4: Rename files
echo "Renaming files..."
find . -depth -name "*tenzura*" | sort -r | while read path; do
    new_path=$(echo "$path" | sed 's/tenzura/tenzura/g; s/Tenzura/Tenzura/g; s/TENZURA/TENZURA/g')
    dir=$(dirname "$new_path")
    mkdir -p "$dir"
    
    if [ -d "$path" ]; then
        # Handle directory moves carefully to avoid conflicts
        if [ "$path" != "$new_path" ]; then
            # Create directory if doesn't exist
            mkdir -p "$new_path"
            # Move contents
            find "$path" -maxdepth 1 -mindepth 1 | while read item; do
                mv "$item" "$new_path/"
            done
            # Remove old directory if it's empty
            rmdir "$path" 2>/dev/null || true
        fi
    else
        # Handle file moves
        if [ -f "$path" ] && [ "$path" != "$new_path" ]; then
            mv "$path" "$new_path"
        fi
    fi
done

# STEP 5: Special cases for build system
echo "Addressing build system files..."
if [ -f "Makefile.am" ]; then
    sed -i 's/\braven/tenzura/g; s/\bRaven/Tenzura/g; s/\bRAVEN/TENZURA/g' Makefile.am
fi

# Update package references
find . -name "*.spec" -o -name "control" | xargs sed -i 's/\braven/tenzura/g; s/\bRaven/Tenzura/g'

# STEP 6: Fix any remaining critical path references
echo "Fixing critical paths..."
find . -type f -not -path "*/\.git/*" -exec sed -i 's/\/\.tenzura\//\/\.tenzura\//g; s/%APPDATA%\\Tenzura/%APPDATA%\\Tenzura/g' {} \;

# STEP 7: Generate list of renamed files for .gitignore updates
echo "Updating .gitignore if present..."
if [ -f ".gitignore" ]; then
    sed -i 's/\braven/tenzura/g; s/\bRaven/Tenzura/g; s/\bRAVEN/TENZURA/g' .gitignore
fi

# STEP 8: Update GitHub workflow files
echo "Updating GitHub workflow files..."
find .github/workflows -type f -name "*.yml" | while read file; do
    sed -i 's/Build Tenzura/Build Tenzura/g; s/build-tenzura/build-tenzura/g' "$file"
    sed -i 's/\braven\b/tenzura/g; s/\bRaven\b/Tenzura/g; s/\bRAVEN\b/TENZURA/g' "$file"
done

# STEP 9: Fix specific files with known patterns
if [ -f "libravenconsensus.pc.in" ]; then
    mv libravenconsensus.pc.in libtenzuraconsensus.pc.in
    sed -i 's/Tenzura consensus/Tenzura consensus/g; s/ravenconsensus/tenzuraconsensus/g' libtenzuraconsensus.pc.in
fi

# Cleanup any temp files
find . -name "*.tmp" -o -name "*.bak" -o -name "*.orig" -o -name "*~" -delete

echo "Rebranding complete!"
echo "Please run 'autoreconf -i' and './configure' to update all generated files."
echo "Then build with 'make' to verify compilation."
