# aubs_cursor

Scripts to create and install the aubs cursor released originally for Windows to some Linux distributions.
Currently there is a script for:

- XFCE (tested on Manjaro)
- KDE (tested on Manjaro)

## Icon/Cursor conversion

The original icons can be found in the directory [`original_icons`](./original_icons).

### X11 cursor

To convert the original `.ani` icons in this directory to `.png` the the script [`xcursorgen/convert_ani_icons_to_png.sh`](xcursorgen/convert_ani_icons_to_png.sh) can be used.
With a program like [gThumb](https://wiki.gnome.org/Apps/Gthumb) these `.ico` (or thanks to `convert` also `.png`) files can be read and exported to another format.
To convert `.png` icons to a X11 cursor format file the program [`xcursorgen`](https://wiki.archlinux.org/title/Xcursorgen) is used using custom cursor config files in the directory [`original_icons`](./original_icons) which also contains for each icon a list of icon output names.
