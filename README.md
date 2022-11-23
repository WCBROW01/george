# george

The name only makes sense if you were there when I was writing it.

It's just a bouncing rectangle implemented in SDL 2.0, similar to those screensavers you see on old set-top boxes.

SDL2_image and SDL2_mixer are also required to compile.

You don't have to load an image or audio file, but you can. By default, it just changes color randomly.

To build, just run `make`.

```
Usage: george -i [IMAGE_FILE] -a [WAV_FILE]
	-h, --help 	Show this help text.
	-i, --image	Load an image to overlay on top of the rectangle.
	-a, --audio	Load a wav file to play when the rectangle bounces.
```
