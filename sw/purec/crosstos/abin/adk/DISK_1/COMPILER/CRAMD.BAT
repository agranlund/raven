cp68 %1.c m:%1.i
c068 m:%1.i m:%1.1 m:%1.2 m:%1.3 -f
rm m:%1.i
c168 m:%1.1 m:%1.2 %1.s
rm m:%1.1
rm m:%1.2
mac -u -6 %1.s
;***********************************************
;*
;* You can use MADMAC for speedy assembly or
;* you can substitute as68 for more optimized
;* assembly code. Please read the MADMAC
;* and as68 manual for other options and 
;* features. Also recommended is a ramdisk for 
;* the intermediate files of compilation.
;*
;* as68 -l -u %1.s
;*
;***********************************************
rm %1.s
wait.prg
