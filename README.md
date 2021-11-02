# aubs_cursor

## Icon conversion

The original icons can be found in the directory [`original_icons`](./original_icons).

To convert the original `.ani` icons in this directory to `.png` the the script [`xfce/convert_ani_icons_to_png.sh`](xfce/convert_ani_icons_to_png.sh) can be used.
With a program like [gThumb](https://wiki.gnome.org/Apps/Gthumb) these `.png` can be read and exported to another format even though the created `.png` file is not in the correct format.

To convert `.png` icons to a X11 cursor format file the program [`xcursorgen`](https://wiki.archlinux.org/title/Xcursorgen) can be used.
The currently created `config.cursor` files need to be edited:

- Replace the first TODO with the icon dimension `32` (pixels)
- Replace the second TODO with the icon time difference to the next with `80` (ms)

Then run: `$ xcursorgen config.cursor output_name`

TODO Automate the creation of a X11 cursor format file
