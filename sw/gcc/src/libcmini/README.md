[![Build Status](https://github.com/freemint/libcmini/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/freemint/libcmini/actions) 

* Latest snapshot: [Download (mint)](https://tho-otto.de/snapshots/libcmini/libcmini-mint-latest.tar.bz2)
* Latest snapshot: [Download (mintelf)](https://tho-otto.de/snapshots/libcmini/libcmini-mintelf-latest.tar.bz2)
* [Archive](https://tho-otto.de/snapshots/libcmini/)

# libcmini

libcmini aims to be a small-footprint C library for the m68k-atari-mint (cross) toolchain, similar to the C library
Pure-C came with. Many GEM programs do not need full MiNT support and only a handful of C library functions.

By default, gcc compiled programs on the Atari ST platform link to mintlib. Mintlib aims to implement all POSIX
functionality to make porting Unix/Linux programs easier (which is a very good thing to have). For small GEM-based
programs, this comes with a drawback, however: program size gets huge due to the underlying UNIX compatibility layer.

For programs that don't need this, libcmini tries to provide an alternative. It's far from finished, but already quite 
usable.

## Example
This small program

```
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello World\r\n");
}
```

compiled with the standard m68k-atari-mint toolchain 

```
m68k-atari-mint-gcc -o hello.tos -s hello.c 
```

results in a binary size of 118220 bytes. Huge. The same thing compiled with libcmini:

```
m68k-atari-mint-gcc -nostdlib $LIBCMINI/crt0.o hello.c -o hello.tos -s -L$LIBCMINI -lcmini -lgcc
```
(note that - since we compile with -nostdlib - you have to add the gcc runtime support library libgcc.a to your command line for the processors that need it.
Sometimes you will have to specify ` -lgcc -lcmini -lgcc` to avoid 'undefined reference' error messages.)

creates a binary with 11794 bytes. About a tenth of the size.

Remember that gcc does not automatically look up multi-lib versions of
libraries in directories specified by -L. If you want to do the same as
above for a different architecture, you have to explicitly add the
correct sub-directory:

```
m68k-atari-mint-gcc -nostdlib -mcpu=5475 $LIBCMINI/crt0.o hello.c -o hello.tos -s -L$LIBCMINI/m5475 -lcmini -lgcc
```

Same applies if you are using any other switch that requires a
different library version, like --mshort and --mfastcall.

## Binary Releases Downloads

binary releases (in tar.gz format) can be downloaded from here: https://github.com/freemint/libcmini/releases
source is also available there.

## What's that libcmini.creator stuff about?

I'm using the QtCreator IDE for my Atari cross development work (most of the time, if I'm not in vi mode). The `libcmini.config`, `libcmini.creator`, `libcmini.files` and `libcmini.includes` files are used by QtCreator to keep track of the project.
If you are not using QtCreator (I would recommend it since it adds a few nice features to Atari cross development), just ignore these files.

## Fastcall?
(contributed by Frederik Olsson)

Fastcall requires [patched GCC 4.6.4](https://github.com/PeyloW/gcc-4.6.4) with -mfastcall support.

GCC passes all arguments on the stack by default. The Atari ST, and even the Falcon 030, have a slow 16 bit wide memory bus.
As a result memory operations are very expensive. Most functions take only a few arguments that can be passed in registers.

-mfastcall uses d0-2/a0-1/fp0-2 for passing integer and pointer arguments when possible, and always return pointers in a0.
a2 is not used for passing arguments, but is considered clobbered, this allows a2 to be used as call target function pointer.

The result is faster code execution, and slightly smaller memory footprint. Especially when used in combination with -mshort to 
avoid library calls for most integer operations.

With -mfastcall libcmini itself is 3% smaller binary, and the tests/bench test executes 20% faster. 

## Compilation

The library can be compiled in standard ('a.out') or `elf' format. In the first case the m68k-atari-mint toolchain is used; in the latter case the m68k-atari-mintelf toolchain. Make sure the proper toolchain is in your PATH before compiling.

The basic command for compilation is `make`. But various compilation options can be given.
Instead of 'Y', also 'yes' can be used:

  * `VERBOSE=Y` - Print all commands that are used to compile the library. This option can also be given as an environment variable. All other options must be given as arguments to the `make` command.
  * `COMPILE_ELF=Y` - Compile for 'elf' format instead of 'a.out'.
  * `ONLY_68K=Y` - Compile only for the 68000, not the other architectures.
  * `STDIO_WITH_LONG_LONG=Y` - Add code for `long long` to the `printf` family of functions. At the moment this fails.

The following options are on by default. They can be switched off with `OPTION=no` or `OPTION=N`.
However, when `ONLY_68K=Y` is given, they are switched off and cannot be switched on again.

  * `BUILD_CF=N` - Don't compile for the Coldfire (m5475) architecture.
  * `BUILD_SOFT_FLOAT=N` - Not sure what this means. Probably don't build for software floating point.
  * `BUILD_SHORT=N` - Don't include libraries for short ints (16 bit?)
  * `BUILD_FAST=N` - Don't use `-mfastcall` function calls. The default for this option is whether the compiler supports `-mfastcall`.
  * `STDIO_MAP_NEWLINE=N` - Don't add code to convert NL to CR NL on output to text files, and CR NL to NL on input. This conversion is standard practice on TOS but not on Mint. If you don't want this then use `STDIO_MAP_NEWLINE=N`, but on TOS your console output may look weird unless you put `\r\n` in your print statements.

Example `make` command:

```
VERBOSE=yes make ONLY_68K=yes STDIO_MAP_NEWLINE=N
```

## Installation

The basic installation command is `make install`. The default install
location is `/usr`, i.e. `/usr/include` and `/usr/lib`. `make install`
should basically get the same options as used for the compilation if
some architectures were omitted, otherwise it will regenerate the
missing ones.

The installation destination can be specified with either the `PREFIX*` options or the `DESTDIR` option:
  * `PREFIX=<dir>` - Install in `$PREFIX/include` and `$PREFIX/lib`, unless one or more of the following options are given:
  * `PREFIX_FOR_INCLUDE=<dir>` - Directory for the include files. When multiple architectures are present,
only one copy of the include files are installed. Default is `$PREFIX/include`.
  * `PREFIX_FOR_LIB=<dir>` - Directory for the library files. Default is `$PREFIX/lib`.
  * `PREFIX_FOR_STARTUP=<dir>` - Directory for the startup file (`crt0.o`). Default is `$PREFIX/lib`.
  * `DESTDIR=<dir` - Installs the include files in `$(DESTDIR)/usr/include` and the library and startup file in
    `$(DESTDIR)/usr/lib`.

The `DESTDIR` and `PREFIX*` options cannot be used together. They can also be given as environment variables.

If the library has been built for multiple architectures, there will be
a subdirectory for each one (except the basic 68000) in the library
directory. For the `elf` version, this applies also to the startup file.

Example `make install` command corresponding to the above compile command:

```
VERBOSE=yes make ONLY_68K=yes PREFIX=/opt/libcmini install
```
The `STDIO_MAP_NEWLINE` option is only relevant at compile time, so this was left out.

## Contribution

Contributions are always welcome.

Please make sure you provide all new code using 'four spaces' indentation without TAB characters and trailing spaces removed.
