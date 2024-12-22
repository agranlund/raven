c:\alcyon\cp68 %1.c %1.i
c:\alcyon\c068 %1.i %1.1 %1.2 %1.3 -f
c:\alcyon\rm %1.i
c:\alcyon\c168 %1.1 %1.2 %1.s
c:\alcyon\rm %1.1
c:\alcyon\rm %1.2
c:\alcyon\as68 -s c:\alcyon\ -l -u %1.s
c:\alcyon\rm %1.s
c:\alcyon\aln -s -o %1.prg -y c:\alcyon\ apstart.o cards.o osbind.o ctype.o vdibind aesbind


