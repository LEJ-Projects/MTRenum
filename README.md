# MTRenum
Renumber program for TRS-80 Model 100 BASIC Programs.

MTRenum is a Windows based console program that I designed for renumbering BASIC programs written for the TRS-80 Model 100, NEC 8201 and perhaps a few other variants.    When executed, the user is asked for an input file name, an output file name and if unnecessary spaces should be eliminated.  If this is selected, spaces will be removed everywhere except again between quotes, in a DATA statement or part of a comment (remark).   

The lines in the input file do not need to be in order.    This is very handy if you want to move subroutines around.  Simply use your text editor to cut and paste the subroutine and then renumber it.   References to the new location will be updated.
And finally, you do not have to have line numbers on your lines unless they are being referenced in another line by the usual line number referencing statements (THEN, ELSE, GOTO, GOSUB, RESTORE, etc.).   This is handy when writing new code.[MTRenum.pdf](https://github.com/LEJ-Projects/MTRenum/files/11103816/MTRenum.pdf)
