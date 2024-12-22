cp68 %1.c m:%1.i
c068 m:%1.i m:%1.1 m:%1.2 m:%1.3 -f
rm m:%1.i
c168 m:%1.1 m:%1.2 %1.s
rm m:%1.1
rm m:%1.2
as68 -l -u %1.s
rm %1.s
wait.prg
