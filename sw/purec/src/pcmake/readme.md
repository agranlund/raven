# What is this?

pcmake is a tool that will let you process Pure-C project files from a
commandline. It will invoke pcc.ttp, pasm.ttp and plink.ttp for this purpose.

# Installation

pcmake.ttp should be installed to the same directory as above mentioned
tools from the Pure-C installation. You should also make sure that this
directory is on your `$PATH`.

# Invocation

pcmake will accept some options compatible to GNU-make:

```
  -B, --always-make        Unconditionally make all targets.
  -C, --directory=DIR      Change to DIRECTORY before doing anything.
  -f, --file=FILE          Read FILE as a project file.
  -s, --silent             Don't echo commands.
  -v, --verbose            Increase verbosity.
  -F, --nfdebug            Echo commands also to emulator, if present.
  -n, --dry-run            Don't actually run any command; just print them.
  -w, --print-directory    Print the current directory.
      --no-print-directory Turn off -w, even if it was turned on implicitly.
  -V, --version            Print the version number and exit.
  -h, --help               Display this help and exit.
```

You can also set the environment variables `$PCCFLAGS`, `$PCASFLAGS` and
`$PCLDFLAGS` to add options that should be passed to the compiler, assembler
and linker, respectively. These will be appended to the corresponding command
lines, independently of what was specified in the project file.

# Search directories

pcmake will use some default directories to locate libraries, and also pass an
additional -I directive to the compiler for the include directory. These are
derived from the path from which pcmake was invoked. Assuming an installation
path of `C:\pure_c`, the directory should therefor look like

```
C:\pure_c\pcmake.ttp
C:\pure_c\pcc.ttp
C:\pure_c\pasm.ttp
C:\pure_c\plink.ttp
C:\pure_c\lib\pcstdlib.lib
C:\pure_c\lib\pctoslib.lib
.. etc
C:\pure_c\include\stdio.h
C:\pure_c\include\string.h
.. etc
```
