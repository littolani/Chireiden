#!/bin/bash

# Usage: ./fix_header_case.sh /path/to/include

cd "$1" || exit 1

find . -type f | while read -r file; do
    dir=$(dirname "$file")
    base=$(basename "$file")
    lowerbase=$(echo "$base" | tr '[:upper:]' '[:lower:]')

    # Only rename if name differs
    if [[ "$base" != "$lowerbase" ]]; then
        echo "Renaming: $file -> $dir/$lowerbase"
        mv "$file" "$dir/$lowerbase"
    fi
done

