#!/usr/bin/env bash

# https://develop.kde.org/docs/features/cursor/#moving-the-files
# Default "icons" folder location
# > For user-specific installation:
#   ~/.local/share/icons/
# > For system-wide installation:
#   /usr/share/icons/
ICON_DIR="$HOME/.local/share/icons"

./install_cursor_x11.sh "$ICON_DIR"
