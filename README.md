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
```

To compile the decompiled game you do the same in the games folder

```bash
cd game_src

make run

# -j flag to run with multiple threads
make -j$(nproc) run
```