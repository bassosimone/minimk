#!/bin/bash

# File: ./scripts/fix_newline_eol.bash
# Purpose: Add newline at EOF for all C/C++ files missing it
# SPDX-License-Identifier: GPL-3.0-or-later
#
set -euo pipefail

find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 | while IFS= read -r -d '' file; do
  if [ -s "$file" ] && [ "$(tail -c1 "$file" | wc -l)" -eq 0 ]; then
    echo "Adding newline to: $file"
    echo >>"$file"
  fi
done
