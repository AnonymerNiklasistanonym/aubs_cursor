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
done
