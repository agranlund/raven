
Castaway/Atari
--------------
    "Makes old crap run even older crap"
    Useful on Atari CT60, Vampire, Raven, or other fast Atari compatibles.
    v241110

    Castaway is distributed under the GPL, version 2 or at your
    option any later version. See license.txt for details.

    This port is based on Castaway/GP32, which is based on CastCE,
    which is based on the original Castaway :)

    Anders Granlund,    Castaway/Atari
    Peter Person,       Castaway/Atari
                        https://github.com/agranlund/castaway

    Jeff Mitchell,      Castaway/GP32
                        http://www.codejedi.com/shadowplan/castaway.html

    Ludovic Olivencia,  CastCE
    Andrew Gower,       CastCE
                        https://castaway.sourceforge.net/castce.html

    Joachim Hoenig,     CaSTaway
                        http://castaway.sourceforge.net



Instructions:
-------------

  This package comes with EmuTOS1.3 but replacing it with an
  official Atari TOS image is highly recommended.

  To do so, put a TOS 1.xx rom image next to castaway.prg and
  and rename it to "tos.img".
  TOS1.2 or TOS1.4 will be most compatible.

  Drag and drop an .ST or .MSA disk image onto castaway.prg to start.
  Alternatively, install castaway as an application that
  automatically launches .ST images when double clicked.


What about multi-disk games?
--------------------------------
  Change floppy disk with CTRL-ALT-1 through 9

  Disk images must have the file extensions .ST, .ST2, .ST3
  and so on for this to work (or .MSA, .MS2, .MS3 etc)

  Example:
     "LSL2.ST", "LSL2.ST2", "LSL2.ST3" for Leisure suit Larry 2


Other controls:
---------------
  CTRL-ALT-ESC: Quit emulator
  CTRL-ALT-T:   Toggles frame-pacing (super turbo mode on Vampires)


Platform specific:
------------------
Atari:
 - Rasters are not yet emulated

Raven:
 - Generally same status as a normal Atari

Firebee:
 - Not yet working

Vampire:
 - Requires V4SA-EmuTOS
 - Requires PSGXBIOS.PRG for sound output

   For general AtariVampire info:
     Atari forum on apollo-core.com
     AtariV4 and ApolloTeam on Discord.


Other clones may or may not work.
Graphics cards are supported through NOVA API and Castaway will try
to use the best suitable mode you have defined in STA_VDI.BIB
For best result you may want to create a low resolution mode
such as 320x200x8bpp or thereabouts.


This sucks, nothing works!
--------------------------
  Bummer.
  This emulator trades accuracy for speed. Demos or games that depend
  on cycle accuracy have very low chance of working.
  
  If a games isn't working which has multiple releases then you might
  want to check if another release might work.
  Compatibility should hopefully improve with future releases.

