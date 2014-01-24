HPE file opener
(c) 2014 Jacek Lipkowski <sq5bpf+hpe@lipkowski.org>

This programs attempts to manipulate HPE (homepatrol scanner files) 
used for Uniden scanners. Works under linux, windows (very lightly tested),
should work under unix and macos (untested).

Usage
-----

Decompression:

hpe_open -d freq.hpe

Creates files freq.gz (deobfuscated compressed data) and freq.gz-dexor 
(decompressed text file).

hpe_open -c freq2

Creates files freq2.gz (compressed data) and freq2.hpe (obfuscated 
compressed data). freq2 should contain a deobfuscated text file.


I have only used this to decompress hpe files found on the internet, and
it seems to work. I don't own a Uniden scanner, and didn't try the 
software. Another person also tried manipulating the text file
and confirmed, that it is possible to compress it again and load it into
the Uniden software. 


File format
------------

These files are nice plain text files, easily manipulated with standard
text utilities (awk, sed, a good text editor etc). The files are compressed
with gzip. For some reason the compressed file is obfuscated with xor 0xc.


Compilation
-----------

A standard Makefile is provided, so just type make. Alternatively use
on unix:

gcc -O2 -lz hpe_open.c -o hpe_open
on windows (the -DWIN32 might not be necsessary, depending on compiler):
gcc -O2 -lz hpe_open.c -o hpe_open -DWIN32

Compiles using gcc. Also tested on windows using mingw (should also work
with cygwin). The zlib library is needed for compilation.


I hope you will find this software useful
73 de Jacek / SQ5BPF

