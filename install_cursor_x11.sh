#!/usr/bin/env bash

ICON_DIR="$1"

SOURCE_XCURSORGEN_DIR="xcursorgen"
SOURCE_X11_CURSORS_DIR="converted_x11_cursors"

# Create X11 cursor icons
cd "$SOURCE_XCURSORGEN_DIR"
./convert_ani_icons_to_png.sh
cd ..

AUBS_CURSOR_THEME_DIR="$ICON_DIR/aubs_cursor"
AUBS_CURSOR_THEME_ICON_DIR="$AUBS_CURSOR_THEME_DIR/cursors"

# Create XFCE icon directory if not existing
mkdir -p "$ICON_DIR"
# Add cursor theme
rm -rf "$AUBS_CURSOR_THEME_DIR"
mkdir "$AUBS_CURSOR_THEME_DIR"
echo "Create '$AUBS_CURSOR_THEME_DIR'"
# > Copy index file
cp "$SOURCE_XCURSORGEN_DIR/index.theme" "$AUBS_CURSOR_THEME_DIR/index.theme"
# > Copy cursor files
mkdir "$AUBS_CURSOR_THEME_ICON_DIR"

for CURSOR_FILE_PATH in "$SOURCE_X11_CURSORS_DIR/"*; do
    CURSOR_NAME=`basename "$CURSOR_FILE_PATH"`
    cp "$CURSOR_FILE_PATH" "$AUBS_CURSOR_THEME_ICON_DIR/$CURSOR_NAME"
done
