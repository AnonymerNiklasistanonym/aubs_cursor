#!/usr/bin/env bash

# Create "ani2png" program
cmake -S aniFileExtractor -B build_cmake
cmake --build build_cmake

# Set input and output paths
ORIGINAL_CURSOR_IMAGES_ANI_DIR="../original_icons"
CONVERTED_CURSOR_IMAGES_PNG_DIR="../converted_png_icons"

mkdir -p "$CONVERTED_CURSOR_IMAGES_PNG_DIR"
for CURSOR_FILE_PATH in "$ORIGINAL_CURSOR_IMAGES_ANI_DIR/"*.ani; do
    CURSOR_NAME=`basename "$CURSOR_FILE_PATH" .ani`
    ./build_cmake/aniFileExtractor "$CURSOR_FILE_PATH" "$CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME"
    for CURSOR_EXTRACTED_ICO_FILE_PATH in "$CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME/"*.ico; do
        CUSRSOR_ICO_FILE_NAME=`basename "$CURSOR_EXTRACTED_ICO_FILE_PATH" .ico`
        convert "$CURSOR_EXTRACTED_ICO_FILE_PATH" "$CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME/$CUSRSOR_ICO_FILE_NAME.png"
    done
    X11_CURSOR_OUTPUT_NAMES_FILE_PATH="$ORIGINAL_CURSOR_IMAGES_ANI_DIR/${CURSOR_NAME}_x11_cursor_names.txt"
    X11_CURSOR_CONFIG_FILE_PATH="$ORIGINAL_CURSOR_IMAGES_ANI_DIR/${CURSOR_NAME}.cursor"
    if test -f "$X11_CURSOR_OUTPUT_NAMES_FILE_PATH"; then
        echo "Found $X11_CURSOR_OUTPUT_NAMES_FILE_PATH -> create X11 icons"
        X11_CURSOR_OUTPUT_NAMES=($(cat "$X11_CURSOR_OUTPUT_NAMES_FILE_PATH"))
        OUTPUT_ABSOLUTE_PATH="`realpath "$CONVERTED_CURSOR_IMAGES_PNG_DIR/x11_cursors"`"
        X11_CURSOR_CONFIG_ABSOLUTE_PATH="`realpath "$X11_CURSOR_CONFIG_FILE_PATH"`"
        mkdir -p "$CONVERTED_CURSOR_IMAGES_PNG_DIR/x11_cursors"
        for i in "${X11_CURSOR_OUTPUT_NAMES[@]}"
        do
            echo "> Create $i"
            CWD=$(pwd)
            cd "$CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME/"
            if test -f "$X11_CURSOR_CONFIG_ABSOLUTE_PATH"; then
                echo ">> Found $X11_CURSOR_CONFIG_FILE_PATH"
                set -x
                xcursorgen "$X11_CURSOR_CONFIG_ABSOLUTE_PATH" "$OUTPUT_ABSOLUTE_PATH/$i"
                set +x
            else
                echo ">> $X11_CURSOR_CONFIG_FILE_PATH not found!"
            fi
            cd "$CWD"
        done
    fi
done
