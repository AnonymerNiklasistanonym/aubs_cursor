#!/usr/bin/env bash

# Create ani2png program
g++ ani2png2xcf/ani2png2xcf.cpp -o ani2png2xcf_NOT_PRODUCTION_READY

# Set input and output paths
ORIGINAL_CURSOR_IMAGES_ANI_DIR="../../original_icons"
CONVERTED_CURSOR_IMAGES_PNG_DIR="../../converted_icons_png2"

mkdir -p "$CONVERTED_CURSOR_IMAGES_PNG_DIR"
for CURSOR_FILE_PATH in "$ORIGINAL_CURSOR_IMAGES_ANI_DIR/"*.ani; do
    CURSOR_NAME=`basename "$CURSOR_FILE_PATH" .ani`
    CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR="$CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME"
    mkdir -p "$CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR"
    cp "$CURSOR_FILE_PATH" "$CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME.ani"
    ./ani2png2xcf_NOT_PRODUCTION_READY "$CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME.ani"
    rm -f "$CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR/$CURSOR_NAME.ani"
    #for PNG_FILE_PATH in "$CURRENT_CONVERTED_CURSOR_IMAGES_PNG_DIR/"*.png; do
    #    convert "$PNG_FILE_PATH" "$PNG_FILE_PATH"
    #done
done
