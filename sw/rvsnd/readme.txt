# rvsnd
Sound HAL for Atari compatibles

Very early work in progress but can currently be used to initialize
cards, as well as control their mixers.
Midi in/out drivers are supported too.


Install:
- put rvsnd.prg in c:\auto
- put rvsnd.inf in root of c:\
- rvsnd.inf contains settings which can be edited with a text editor of choice

Remove the old standalone drivers from your auto folder if you have them:
- mpu401.prg
- oplmidi.prg
- ultrinit.prg


You can use rvsctrl.prg on the commandline to view and change settings.
(They are not saved. for that you need to manually edit rvsnd.inf)

The gsxbmixer application should work with rvsnd:
https://assemsoft.atari.org/gsxb/index.html


todo: proper documentation...
