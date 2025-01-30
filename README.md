# Spyro decompiler

An attempt at writing a decompiler specifically for the game [Spyro the Dragon](https://en.wikipedia.org/wiki/Spyro_the_Dragon).
The decompiler is a bit annoying to work with, but I was able to complete the entire game using it so I see that as a win.

## sources

- <https://psx.arthus.net/docs/MIPS%20Instruction%20Set-harvard.pdf>
- <https://problemkaputt.de/psxspx-contents.htm>
- psy-q sdk

## things that don't work

- graphical issues, seeing rainbows when the camera is close to the floor/grass?
- demo mode
- no sound

## how to compile

The target platform is linux, it may work on windows with mingw or wsl.
make sure you have gcc, make and SDL2 (SDL2 with header files) installed.
If you have apt you can install it with this command, but i have not tested it

```bash
sudo apt install build-essentials libsdl2-dev
```

Before you compile anything you need to edit config.h to point to the disk file, the programs expect a .bin file from a .cue/.bin pair.
The file name can be relative to the git folder, you need to force a recompile after changing the config file (or any .h file) later.

```c
#define ROM_NAME "spyro.bin"
```

To compile the decompiler all you need to do is run make in the program folder, or make run to immediately run it

```bash
cd decompilation

make run

# to return to git folder
cd ..
```

To compile the decompiled game you do the same in the games folder

```bash
cd game_src

make run

# -j flag to run with multiple threads
make -j$(nproc) run
```

## controls

- d-pad: wasd
- square: left shift
- circle: e
- cross: space
- triangle: q
- l1: y
- r1: u
- l2: h
- r2: j
- start: enter

## license

I don't know much about licenses, Hopefully I chose a good one.

## compilation targets

You will not be able to run the program on a playstation ironically.
You would have to replace all calls to LW and SW etc. functions before we can consider adding playstation support, which is a nearly impossible task.
It should run perfectly fine on your local machine though, and you can modify the source code and decompiled code however you want.
Keep in mind the decompiled code gets overwritten once you rerun the decompiler, I can add a guide in the future for how to avoid this.