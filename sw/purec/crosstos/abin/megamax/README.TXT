
There are three additional text files on this disk:

ADDENDUM.TXT    -   Corrections to the manual.

CONVERT.TXT     -   Information for converting between previous versions of
                    Megamax C and the new Laser C.

DEBUGGER.TXT    -   contains the documentation for the symbolic debugger.


Miscellaneous Tips, Hints and Questions.
----------------------------------------

Don't use a ram disk. If you have a 1040 ST the best way to use ram is to let
the shell manage it. The shell uses a dynamic disk cache, allocating as much 
memory as it needs and deallocating it when a program needs it. Therefore it
is quite efficient.

If the compiler complains that it can't find the include files check to make
sure that the environment variable CINCLUDE is set to the correct path and disk.

If you get a link error that refers to a label "errno" you may not be linking
in the file INIT.O.  This file is placed automatically at the beginning of a
link when you use either the RUN command or the CC utility.  INIT.O MUST be
the first object file given to the linker.

The shell replaces various DOS function calls with emulated versions that
do the same thing.  While every attempt has been made to ensure compatibility
with the ROM versions, we cannot guaranty that it is 100% compatible.  You
should therefore thoroughly test any software you plan to release OUTSIDE
the laser shell (Like from the desktop).

Development is possiple with only 512k of RAM, however, you may find yourself
running out of memory. If this is the case there are two options open to you:

    1. Get more RAM.

    2. If running out of memory during compiles, split your source code into
    smaller files.  Examine the files that you are #incude'ing, they may not
    all be neccessary (or you may only need parts of them  -- gemdefs.h is
    rather large).  If running out of memory during the link, see item 1.



    You may find that your old projects produce larger executable programs
    than those produced by Laser C.  With the switch to generation of
    absolute code (rather than PC-relative code) you may find that your code
    size is increased. This is because 32 bit references are being made
    instead of 16 bit references. You can combat this by doing a few things.

    1. Limit your global variables. Global and static variables use the 32 bit
    absolute addressing. Changing these variables to local variables, where
    possible, will increase your speed and decrease your size. It may also
    make your program easier to debug, since your program will be more modular
    and structured.

    2. Use register variables. There are four data and four address registers
    available. If you are referencing a global array or variable more than
    four times (or it is being referenced in a loop) then assign the global
    to a register variable.  You will see a very pronounced increase in speed
    and a decrease in code size.  With a global array you can do something like
    this:

        typedef int int_array[100];
        int_array global_array;

        a_function(i)
            register int i;
        {
            register int_array *global_array_ptr = (int_array *) global_array;

            (*ptr_to_global_array)[i] = 4;
        }


    3. If you create archives (libraries) you will want to split the source
    files up as much as possible. This is because the linker can no longer
    extract individual functions from an archive due to the new archive
    format.

